#include <cstdint>
#pragma once

namespace msk::ir {
template <typename T> class Identifiable {
private:
  inline static uint32_t next_id = 0;

public:
  uint32_t id;
  Identifiable() : id(++next_id) {};
};
} // namespace msk::ir
