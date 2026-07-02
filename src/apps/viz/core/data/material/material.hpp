#include <memory>
#pragma ONCE

namespace ntg::viz {

class Pipeline;

typedef struct Material {
  std::shared_ptr<Pipeline> pipeline;
  // TODO: Add in textures
} Material;

} // namespace ntg::viz
