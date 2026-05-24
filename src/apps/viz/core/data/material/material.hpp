#include <memory>
#pragma ONCE

namespace ntg::viz {
class Pipeline;

struct Material {
  std::shared_ptr<Pipeline> pipeline;
};
} // namespace ntg::viz
