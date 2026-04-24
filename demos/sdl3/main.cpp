#include <spdlog/spdlog.h>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static ImGuiIO *io = nullptr;

bool trySDLFunc(bool ret) {
  if (!ret)
    spdlog::error("SDL Error: ", SDL_GetError());
  return ret;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  spdlog::info("Setting up SDL");
  SDL_SetAppMetadata("sdl3test", "1.0", "node2glsl.demos.sdl3test");

  if (!trySDLFunc(SDL_Init(SDL_INIT_VIDEO)))
    return SDL_APP_FAILURE;

  spdlog::info("Creating Window and Renderer");
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  if (!trySDLFunc(SDL_CreateWindowAndRenderer(
          "sdl3test", static_cast<int>(800 * main_scale),
          static_cast<int>(600 * main_scale),
          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY |
              SDL_WINDOW_HIDDEN,
          &window, &renderer)))
    return SDL_APP_FAILURE;
  // SDL_SetRenderLogicalPresentation(renderer, 800, 600,
  //                                  SDL_LOGICAL_PRESENTATION_LETTERBOX);

  SDL_SetRenderVSync(renderer, 1);
  SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(window);

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
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

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

  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

  ImGui::Render();
  SDL_SetRenderScale(renderer, io->DisplayFramebufferScale.x,
                     io->DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(renderer, 0.1f, 0.1f, 0.1f, 1.0f);
  SDL_RenderClear(renderer);
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
  SDL_RenderPresent(renderer);

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  spdlog::info("Quitting SDL");
}
