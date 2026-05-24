#pragma ONCE
#include <viz/core/util/gpu/gpu.hpp>
#include <viz/core/util/shader/shader.hpp>

namespace ntg::viz {
class Context {
public:
  GPUWrapper *gpu;

public:
  Context() { this->gpu = new GPUWrapper(); }

  ~Context() { delete this->gpu; }
};
} // namespace ntg::viz
