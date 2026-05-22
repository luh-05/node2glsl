#pragma ONCE
#include <viz/core/gpu.hpp>
#include <viz/core/shader/shader.hpp>

namespace ntg::viz {
class Context {
public:
  GPUWrapper *gpu;

public:
  Context() { this->gpu = new GPUWrapper(); }

  ~Context() { delete this->gpu; }
};
} // namespace ntg::viz
