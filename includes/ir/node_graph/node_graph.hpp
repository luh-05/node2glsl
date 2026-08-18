#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#pragma ONCE

namespace msk::ir {
typedef struct Slot {
  uint32_t id;
  // TODO: change to enum
  char *dataType;
} Slot;

class CodegenToken;
/**
 * @brief Node representation in Graph
 */
typedef class Node {
  virtual std::vector<std::unique_ptr<CodegenToken>> GenerateTokenString() = 0;
} Node;

/**
 * @brief Information on used definitions in a Module
 * @param constants vector of used constants
 * @param input_ports vector of used ports for input
 * @param output_ports vector of used ports for output
 */
typedef struct FetchInfo {
  const std::vector<std::string> constants;
  const std::vector<std::string> input_ports;
  const std::vector<std::string> output_ports;
} FetchInfo;

/**
 * @brief Specification of Node for Modules
 */
typedef class Module : Node {
  virtual FetchInfo GetFetchInfo();
} Module;

/**
 * @brief Graph Node
 */
typedef class Graph : Node {
private:
  std::vector<std::unique_ptr<Node>> subnodes();

public:
} Graph;

/**
 * @brief Graph Connection
 */
typedef struct Connection {
  std::unique_ptr<Slot> from_slot;
  std::unique_ptr<Slot> to_slot;
} Connection;

/**
 * @brief OO Graph representation
 */
typedef class NodeGraph {
private:
  std::unique_ptr<Graph> graph();
} NodeGraph;
} // namespace msk::ir
