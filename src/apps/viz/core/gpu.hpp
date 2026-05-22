#pragma ONCE
#include "absl/status/statusor.h"
#include <absl/status/status.h>
#include <memory>
#include <viz/core/pipeline.hpp>

namespace ntg::viz {
enum class ShaderFormat { SpirV, Dxil, Metal };
enum class SwapchainComposition {
  Sdr,
  SdrLinear,
  HdrExtendedLinear,
  Hdr10St2084
};
enum class PresentMode { VSync, Immediate, Mailbox };

// TODO: Remove this later once SDL is decoupled
typedef uint32_t *Window;

class GPUWrapper {
public:
  GPUWrapper();
  ~GPUWrapper();

  auto CreateGPUDevice(ShaderFormat format, char *driver) -> absl::Status;
  auto GetDriverName() -> absl::StatusOr<std::string_view>;
  auto ClaimWindow(Window window) -> absl::Status;
  auto SetSwapchainParameters(Window window, SwapchainComposition composition,
                              PresentMode presentMode) -> absl::Status;
  auto ReleaseShader(std::shared_ptr<Shader> shader) -> void;
  // TODO: Remove later
  auto RawGetDevice() -> void *;

private:
  struct Impl;
  std::unique_ptr<Impl> pimpl_;
};
} // namespace ntg::viz
