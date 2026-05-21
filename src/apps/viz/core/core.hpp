#pragma ONCE
#include "gpu.hpp"
#include "shader/shader.hpp"

namespace ntg::viz {
class Context {
public:
  GPUWrapper *gpu;

public:
  Context() { this->gpu = new GPUWrapper(); }

  ~Context() { delete this->gpu; }
};
} // namespace ntg::viz
