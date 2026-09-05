#pragma once
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <cstdint>
#include <iterator>
#include <map>
#include <memory>
#include <mir/utils.hpp>
#include <string>
#include <vector>

namespace msk::ir {
class Connection;
typedef struct Port : public Identifiable<Port> {
  // TODO: change to enum
  std::string dataType; // Data type of Port

  std::shared_ptr<Connection> connection;

  Port(std::string dataType) : dataType(dataType) {};
} Slot;

class CodegenToken; // pimpl
/**
 * @brief Node representation in Graph
 */
typedef class Node : public Identifiable<Node> {
public:
  // Left and right ports, maps name to port
  std::map<std::string, std::unique_ptr<Port>> leftPorts;
  std::map<std::string, std::unique_ptr<Port>> rightPorts;
} Node;

class GraphContext; // pimpl
/**
 * @brief Connector between internal GraphContext and interface for Module
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
// template <std::output_iterator<Token> Out>
typedef class Module : public Node {
public:
  using Token = std::unique_ptr<CodegenToken>;
  using Out = std::back_insert_iterator<std::vector<Token>>;

  // Generates CodegenTokens
  virtual auto GenerateTokenString(ContextProvider *context, Out out)
      -> absl::Status = 0;
} Module;

/**
 * @brief Graph Node
 */
typedef class Graph : public Node {
private:
  std::vector<std::unique_ptr<Node>> subnodes;

public:
} Graph;

/**
 * @brief Graph Connection
 */
typedef struct Connection : public Identifiable<Connection> {
  // Left and right port of the connection
  Port *left_port;
  Port *right_port;
} Connection;
} // namespace msk::ir
