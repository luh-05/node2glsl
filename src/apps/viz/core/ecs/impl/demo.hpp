#include "spdlog/spdlog.h"
#pragma ONCE
#include "../ecs.hpp"

namespace ntg::viz {
class TestSystem : public System {
  inline void entityRegistered(entity_t entitiy) override {
    spdlog::debug("Entity registered: {}", entitiy);
  }
  inline void entityErased(entity_t entitiy) override {
    spdlog::debug("Entity removed: {}", entitiy);
  }
};

typedef struct TestComponent {

} TestComponent;
} // namespace ntg::viz
