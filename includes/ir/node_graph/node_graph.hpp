#include <cstdint>
#pragma ONCE

namespace msk::ir {
typedef struct Slot {
  uint32_t id;
  // TODO: change to enum
  char *dataType;
} Slot;

typedef class Node {

} Node;

typedef class NodeGraph {

} NodeGraph;
} // namespace msk::ir
