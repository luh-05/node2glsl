#include "../utils.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>
#pragma ONCE

namespace msk::ir {
class Connection;
typedef struct Port : Identifiable<Port> {
  // TODO: change to enum
  std::string dataType; // Data type of Port

  std::shared_ptr<Connection> connection;

  Port(std::string dataType) : dataType(dataType) {};
} Slot;

class CodegenToken; // pimpl
/**
 * @brief Node representation in Graph
 */
typedef class Node : Identifiable<Node> {
private:
  // Left and right ports, maps name to port
  std::map<std::string, Port> leftPorts;
  std::map<std::string, Port> rightPorts;
} Node;

class GraphContext; // pimpl
/**
 * @brief Connecter between internal GraphContext and interface for Module
 * specification
 */
typedef class ContextProvider {
private:
  std::unique_ptr<GraphContext> context;

public:
  // Gets the named constant of the provided node
  template <typename T>
  auto GetConstant(Node *n, std::string name) -> absl::StatusOr<T>;
} ContextProvider;

/**
 * @brief Specification of Node for Modules
 */
typedef class Module : Node {
  // Generates a CodegenToken vector
  virtual std::vector<std::unique_ptr<CodegenToken>>
  GenerateTokenString(ContextProvider context) = 0;
} Module;

/**
 * @brief Graph Node
 */
typedef class Graph : Node {
private:
  std::vector<std::unique_ptr<Node>> subnodes;

public:
} Graph;

/**
 * @brief Graph Connection
 */
typedef struct Connection : Identifiable<Connection> {
  // Left and right port of the connection
  std::unique_ptr<Port> left_port;
  std::unique_ptr<Port> right_port;
} Connection;
} // namespace msk::ir
