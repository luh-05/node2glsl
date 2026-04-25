#include "shader.h"
#include "spdlog/common.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
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
ABSL_FLAG(std::string, log_level, "info", "logging level");

static SDL_Window *window = nullptr;
static SDL_GPUDevice *device = nullptr;
static ImGuiIO *io = nullptr;
static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static ntg::viz::Shader *vertex_shader = nullptr;
static ntg::viz::Shader *fragment_shader = nullptr;
static SDL_GPUGraphicsPipeline *pipeline = nullptr;
static SDL_GPUBuffer *index_buffer = nullptr;

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

  const std::string fll = absl::GetFlag(FLAGS_log_level);
  if (strcmp(fll.c_str(), "info") == 0) {
    spdlog::set_level(spdlog::level::info);
  } else if (strcmp(fll.c_str(), "debug") == 0) {
    spdlog::set_level(spdlog::level::debug);
  } else {
    spdlog::error("Log level '{}' not recognized. Defaulting to 'info'.", fll);
    spdlog::set_level(spdlog::level::info);
  }

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

  vertex_shader = new ntg::viz::Shader(device);
  ntg::viz::Shader::ShaderAttribs vertex_shader_attribs;
  vertex_shader_attribs.num_samplers = 0;
  vertex_shader_attribs.num_storage_textures = 0;
  vertex_shader_attribs.num_storage_buffers = 0;
  vertex_shader_attribs.num_uniform_buffers = 0;
  vertex_shader_attribs.props = 0;
  if (!vertex_shader->loadShaderFromFile("./src/viewer/shader/def.vert",
                                         "def.vert", ntg::viz::SPIRV_VERTEX,
                                         &vertex_shader_attribs)) {
    spdlog::error("An error occured whilst loading shaders!");
    return SDL_APP_FAILURE;
  }

  fragment_shader = new ntg::viz::Shader(device);
  ntg::viz::Shader::ShaderAttribs fragment_shader_attribs;
  fragment_shader_attribs.num_samplers = 0;
  fragment_shader_attribs.num_storage_textures = 0;
  fragment_shader_attribs.num_storage_buffers = 0;
  fragment_shader_attribs.num_uniform_buffers = 0;
  fragment_shader_attribs.props = 0;
  if (!fragment_shader->loadShaderFromFile("./src/viewer/shader/def.frag",
                                           "def.frag", ntg::viz::SPIRV_FRAGMENT,
                                           &fragment_shader_attribs)) {
    spdlog::error("An error occured whilst loading shaders!");
    return SDL_APP_FAILURE;
  }

  spdlog::info("Creating graphics pipeline");
  SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info;
  pipeline_create_info.vertex_shader = vertex_shader->getShader();
  pipeline_create_info.fragment_shader = fragment_shader->getShader();
  SDL_GPUVertexInputState vertex_input_state;
  vertex_input_state.vertex_buffer_descriptions = nullptr;
  vertex_input_state.num_vertex_buffers = 0;
  vertex_input_state.vertex_attributes = nullptr;
  vertex_input_state.num_vertex_attributes = 0;
  pipeline_create_info.vertex_input_state = vertex_input_state;
  pipeline_create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
  SDL_GPURasterizerState rasterizer_state;
  rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
  rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
  rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;
  rasterizer_state.depth_bias_constant_factor = 1.0f;
  rasterizer_state.depth_bias_clamp = 0.0f;
  rasterizer_state.depth_bias_slope_factor = 1.0f;
  rasterizer_state.enable_depth_bias = false;
  rasterizer_state.enable_depth_clip = false;
  rasterizer_state.padding1 = 0;
  rasterizer_state.padding2 = 0;
  pipeline_create_info.rasterizer_state = rasterizer_state;
  SDL_GPUMultisampleState multisample_state;
  multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;
  multisample_state.sample_mask = 0;
  multisample_state.enable_mask = false;
  multisample_state.enable_alpha_to_coverage = false;
  multisample_state.padding2 = 0;
  multisample_state.padding3 = 0;
  pipeline_create_info.multisample_state = multisample_state;
  SDL_GPUDepthStencilState depth_stencil_state;
  depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS;
  SDL_GPUStencilOpState depth_stencil_op;
  depth_stencil_op.fail_op = SDL_GPU_STENCILOP_KEEP;
  depth_stencil_op.pass_op = SDL_GPU_STENCILOP_REPLACE;
  depth_stencil_op.depth_fail_op = SDL_GPU_STENCILOP_KEEP;
  depth_stencil_op.compare_op = SDL_GPU_COMPAREOP_NEVER;
  depth_stencil_state.back_stencil_state = depth_stencil_op;
  depth_stencil_state.front_stencil_state = depth_stencil_op;
  depth_stencil_state.compare_mask = 0b0;
  depth_stencil_state.write_mask = 0b0;
  depth_stencil_state.enable_depth_test = false;
  depth_stencil_state.enable_depth_write = false;
  depth_stencil_state.enable_stencil_test = false;
  depth_stencil_state.padding1 = 0;
  depth_stencil_state.padding2 = 0;
  depth_stencil_state.padding3 = 0;
  pipeline_create_info.depth_stencil_state = depth_stencil_state;
  SDL_GPUGraphicsPipelineTargetInfo target_info;
  SDL_GPUColorTargetDescription color_target_description;
  color_target_description.format = SDL_GPU_TEXTUREFORMAT_B8G8R8A8_UNORM;
  SDL_GPUColorTargetBlendState color_target_blend_state;
  color_target_blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
  color_target_blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
  color_target_blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
  color_target_blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
  color_target_blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
  color_target_blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
  color_target_blend_state.color_write_mask =
      SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G |
      SDL_GPU_COLORCOMPONENT_B | SDL_GPU_COLORCOMPONENT_A;
  color_target_blend_state.enable_blend = false;
  color_target_blend_state.enable_color_write_mask = false;
  color_target_blend_state.padding1 = 0;
  color_target_blend_state.padding2 = 0;
  color_target_description.blend_state = color_target_blend_state;
  target_info.color_target_descriptions = &color_target_description;
  target_info.num_color_targets = 1;
  target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UINT;
  target_info.has_depth_stencil_target = false;
  target_info.padding1 = 0;
  target_info.padding2 = 0;
  target_info.padding3 = 0;
  pipeline_create_info.target_info = target_info;
  pipeline_create_info.props = 0;

  pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_create_info);
  if (pipeline == nullptr) {
    spdlog::error("Failed to create graphics pipeline: {}", SDL_GetError());
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

void renderRaster() {
  SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUTexture *swapchain_texture;
  SDL_WaitAndAcquireGPUSwapchainTexture(command_buffer, window,
                                        &swapchain_texture, nullptr, nullptr);
  if (swapchain_texture != nullptr) {

    SDL_GPUBufferBinding buffer_binding;
    SDL_GPUBufferCreateInfo buffer_create_info;
    buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    buffer_create_info.size = 3 * 4;
    buffer_create_info.props = 0;
    index_buffer = SDL_CreateGPUBuffer(device, &buffer_create_info);
    if (index_buffer == nullptr) {
      spdlog::error("Failed to create index buffer!");
    }
    SDL_GPUTransferBufferCreateInfo gpu_transfer_buffer_create_info;
    gpu_transfer_buffer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    gpu_transfer_buffer_create_info.size = 3 * 4;
    SDL_GPUTransferBuffer *transfer_buffer =
        SDL_CreateGPUTransferBuffer(device, &gpu_transfer_buffer_create_info);
    void *transfer_buffer_pointer =
        SDL_MapGPUTransferBuffer(device, transfer_buffer, false);
    int *index_buffer_data = reinterpret_cast<int *>(transfer_buffer_pointer);
    index_buffer_data[0] = 0;
    index_buffer_data[1] = 1;
    index_buffer_data[2] = 2;
    buffer_binding.buffer = index_buffer;
    buffer_binding.offset = 0;
    SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer);

    SDL_GPUTransferBufferLocation gpu_transfer_buffer_location;
    gpu_transfer_buffer_location.transfer_buffer = transfer_buffer;
    gpu_transfer_buffer_location.offset = 0;
    SDL_GPUBufferRegion gpu_buffer_region;
    gpu_buffer_region.buffer = index_buffer;
    gpu_buffer_region.offset = 0;
    gpu_buffer_region.size = 3 * 4;
    SDL_UploadToGPUBuffer(copy_pass, &gpu_transfer_buffer_location,
                          &gpu_buffer_region, false);
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer);

    SDL_EndGPUCopyPass(copy_pass);

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
    SDL_BindGPUGraphicsPipeline(render_pass, pipeline);
    SDL_BindGPUIndexBuffer(render_pass, &buffer_binding,
                           SDL_GPU_INDEXELEMENTSIZE_32BIT);
    SDL_DrawGPUIndexedPrimitives(render_pass, 3, 1, 0, 0, 0);
    SDL_EndGPURenderPass(render_pass);

    SDL_ReleaseGPUBuffer(device, index_buffer);
    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    // imgui
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();
    const bool is_minimized =
        (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);

    if (!is_minimized) {
      // Upload vertex/index buffers
      ImGui_ImplSDLGPU3_PrepareDrawData(draw_data, command_buffer);

      // Setup render pass
      target_info = {};
      target_info.texture = swapchain_texture;
      target_info.clear_color =
          SDL_FColor{clear_color.x, clear_color.y, clear_color.z, 0.0f};
      target_info.load_op = SDL_GPU_LOADOP_LOAD;
      target_info.store_op = SDL_GPU_STOREOP_STORE;
      target_info.mip_level = 0;
      target_info.layer_or_depth_plane = 0;
      target_info.cycle = false;
      render_pass =
          SDL_BeginGPURenderPass(command_buffer, &target_info, 1, nullptr);

      // Render imgui pass
      ImGui_ImplSDLGPU3_RenderDrawData(draw_data, command_buffer, render_pass);

      SDL_EndGPURenderPass(render_pass);
    }
    SDL_SubmitGPUCommandBuffer(command_buffer);
  }
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  renderRaster();

  // Submit command buffer

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  spdlog::info("Quitting SDL");
  if (index_buffer != nullptr) {
  }

  if (pipeline != nullptr) {
    SDL_ReleaseGPUGraphicsPipeline(device, pipeline);
  }

  if (vertex_shader != nullptr) {
    delete vertex_shader;
  }
  if (fragment_shader != nullptr) {
    delete fragment_shader;
  }

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
