#include <SDL3/SDL_error.h>
#include <cstdint>
#include <string.h>

#include <SDL3/SDL_gpu.h>
#include <spdlog/spdlog.h>
#include <vector>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_video.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

#include <shaderc/shaderc.h>
#include <shaderc/shaderc.hpp>
#include <shaderc/status.h>

ABSL_FLAG(std::optional<std::string>, gpu_driver, std::nullopt,
          "gpu driver to use");

static SDL_Window *window = nullptr;
static SDL_GPUDevice *device = nullptr;
static ImGuiIO *io = nullptr;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void logSDLError() { spdlog::error("SDL Error: {}", SDL_GetError()); }

#define TRY_SDL(func)                                                          \
  if (!func) {                                                                 \
    logSDLError();                                                             \
    return SDL_APP_FAILURE;                                                    \
  }

std::vector<uint32_t> compileGLSLToSpv(const std::string &source,
                                       shaderc_shader_kind kind,
                                       const char *filename) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  options.SetOptimizationLevel(shaderc_optimization_level_performance);

  shaderc::SpvCompilationResult module =
      compiler.CompileGlslToSpv(source, kind, filename, options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
    spdlog::error("Shaderc error: {}", module.GetErrorMessage());
    return std::vector<uint32_t>();
  }

  return {module.cbegin(), module.cend()};
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  // Abseil flags setup
  absl::SetProgramUsageMessage("Simple pathtracer for volumetric shaders\n"
                               "Usage: viewer [options]");
  absl::ParseCommandLine(argc, argv);

  // SDL Setup
  spdlog::info("Setting up SDL");
  SDL_SetAppMetadata("sdl3gpu", "1.0", "node2glsl.demos.sdl3gpu");

  TRY_SDL(SDL_Init(SDL_INIT_VIDEO));

  spdlog::info("Creating Window");
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  window = SDL_CreateWindow(
      "sdl3gpu", static_cast<int>(800 * main_scale),
      static_cast<int>(600 * main_scale),
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_HIDDEN);
  if (window == nullptr) {
    logSDLError();
    return SDL_APP_FAILURE;
  }

  // Set up GPU device
  spdlog::info("Setting up GPU Device");

  char *driver = nullptr;
  SDL_GPUShaderFormat shader_format =
      SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL |
      SDL_GPU_SHADERFORMAT_MSL | SDL_GPU_SHADERFORMAT_METALLIB;
  if (absl::GetFlag(FLAGS_gpu_driver).has_value()) {
    std::string driver_flag = absl::GetFlag(FLAGS_gpu_driver).value();
    driver = driver_flag.data();
    if (strcmp(driver, "vulkan") == 0) {
      shader_format = SDL_GPU_SHADERFORMAT_SPIRV;
    } else if (strcmp(driver, "metal") == 0) {
      shader_format = SDL_GPU_SHADERFORMAT_METALLIB | SDL_GPU_SHADERFORMAT_MSL;
    } else if (strcmp(driver, "direct3d12") == 0) {
      shader_format = SDL_GPU_SHADERFORMAT_DXIL;
    } else {
      spdlog::error("Driver '{}' is not valid", driver);
      return SDL_APP_FAILURE;
    }
  }

  device = SDL_CreateGPUDevice(shader_format, true, driver);
  if (device == nullptr) {
    logSDLError();
    return SDL_APP_FAILURE;
  }

  if (!absl::GetFlag(FLAGS_gpu_driver).has_value()) {
    spdlog::warn("No driver provided!");
  }
  spdlog::info("Using driver '{}'", SDL_GetGPUDeviceDriver(device));

  TRY_SDL(SDL_ClaimWindowForGPUDevice(device, window));
  SDL_SetGPUSwapchainParameters(device, window,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
                                SDL_GPU_PRESENTMODE_VSYNC);

  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

  spdlog::info("Compiling shaders");
  std::string vertex_code = "#version 450 \n\
\n\
layout(location = 0) in vec3 inPosition; \n\
\n\
void main() { \n\
  gl_Position = vec4(inPosition, 1.0); \n\
} ";
  spdlog::info("Vertex shader code:\n {}", vertex_code);
  std::vector<uint32_t> vertex_spv =
      compileGLSLToSpv(vertex_code, shaderc_vertex_shader, "./shader/def.vert");
  SDL_GPUShaderCreateInfo vertex_shader_create_info;
  vertex_shader_create_info.code_size = vertex_spv.size() * sizeof(uint32_t);
  vertex_shader_create_info.code =
      reinterpret_cast<const uint8_t *>(vertex_spv.data());
  vertex_shader_create_info.entrypoint = "main";
  vertex_shader_create_info.format = shader_format;
  vertex_shader_create_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
  vertex_shader_create_info.num_samplers = 0;
  vertex_shader_create_info.num_storage_textures = 0;
  vertex_shader_create_info.num_storage_buffers = 0;
  vertex_shader_create_info.num_uniform_buffers = 1;
  vertex_shader_create_info.props = 0;
  SDL_GPUShader *vertex_shader =
      SDL_CreateGPUShader(device, &vertex_shader_create_info);
  if (vertex_shader == nullptr) {
    spdlog::error("Couldn't compile shaders: {}", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  spdlog::info("Setting up imgui");
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = &ImGui::GetIO();
  (void)io;
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(
      main_scale); // Bake a fixed style scale. (until we have a solution for
                   // dynamic style scaling, changing this requires resetting
                   // Style + calling this again)
  style.FontScaleDpi =
      main_scale; // Set initial font scale. (in docking branch: using
                  // io.ConfigDpiScaleFonts=true automatically overrides this
                  // for every window depending on the current monitor)

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLGPU(window);
  ImGui_ImplSDLGPU3_InitInfo init_info = {};
  init_info.Device = device;
  init_info.ColorTargetFormat =
      SDL_GetGPUSwapchainTextureFormat(device, window);
  init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;
  init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
  init_info.PresentMode = SDL_GPU_PRESENTMODE_VSYNC;
  ImGui_ImplSDLGPU3_Init(&init_info);

  spdlog::info("Starting render & event loop");

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  ImGui_ImplSDL3_ProcessEvent(event);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

  ImGui_ImplSDLGPU3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

  ImGui::Render();
  ImDrawData *draw_data = ImGui::GetDrawData();
  const bool is_minimized =
      (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

  SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);

  SDL_GPUTexture *swapchain_texture;
  SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window,
                                        &swapchain_texture, nullptr, nullptr);

  if (swapchain_texture != nullptr && !is_minimized) {
    // Upload vertex/index buffers
    ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

    // Setup render pass
    SDL_GPUColorTargetInfo target_info = {};
    target_info.texture = swapchain_texture;
    target_info.clear_color =
        SDL_FColor{clear_color.x, clear_color.y, clear_color.z, clear_color.w};
    target_info.load_op = SDL_GPU_LOADOP_CLEAR;
    target_info.store_op = SDL_GPU_STOREOP_STORE;
    target_info.mip_level = 0;
    target_info.layer_or_depth_plane = 0;
    target_info.cycle = false;
    SDL_GPURenderPass *render_pass =
        SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

    // Render imgui pass
    ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

    SDL_EndGPURenderPass(render_pass);
  }

  // Submit command buffer
  SDL_SubmitGPUCommandBuffer(command_buffer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  spdlog::info("Quitting SDL");
  if (device != nullptr) {
    SDL_WaitForGPUIdle(device);
    ImGui_ImplSDL3_Shutdown();
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui::DestroyContext();

    SDL_ReleaseWindowFromGPUDevice(device, window);
    SDL_DestroyGPUDevice(device);
  }
  if (window != nullptr) {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();
}
