#pragma once
#include "mir/codegen.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <concepts>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <mir/utils.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <tuple>
#include <utility>
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
class Node : public Identifiable<Node> {
public:
  // Left and right ports, maps name to port
  std::map<std::string_view, std::unique_ptr<Port>> leftPorts;
  std::map<std::string_view, std::unique_ptr<Port>> rightPorts;
};

class GraphContext; // pimpl
/**
 * @brief Connector between internal GraphContext and interface for Module
 * specification
 */
class ContextProvider {
private:
  std::unique_ptr<GraphContext> context;

public:
  // Gets the named constant of the provided node
  template <typename T>
  auto GetConstant(Node *n, std::string name) -> absl::StatusOr<T>;
};

/**
 * @brief Specification of Node for Modules
 */
class Module : public Node {
public:
  using Token = std::unique_ptr<CodegenToken>;

  class Out {
  public:
    using Inserter = std::back_insert_iterator<std::vector<Token>>;
    Inserter &it;
    Module &parent;

    Out(Inserter &&it, Module &parent) : it(it), parent(parent) {};
    ~Out() {
      if (!cur_line.empty())
        flushLine();
    }

    enum class Polarity { LEFT = 0x0, RIGHT = 0x1 };
    using PortFetch = std::tuple<Polarity, std::string_view &>;

    std::string cur_line;
    template <class T> void appendLine(T s) { this->cur_line += s; }

    void flushLine() {
      *this->it = std::make_unique<TextToken>(cur_line);
      cur_line = "";
    }

    Out &operator+(PortFetch fetch) {
      auto [p, name] = fetch;

      auto *map = &this->parent.leftPorts;
      if (Polarity::RIGHT == p)
        map = &this->parent.rightPorts;

      if (!map->contains(name)) {
        // TODO: implement ErrorToken
        // return *this;
      }
      flushLine();
      *(this->it) =
          std::make_unique<WildcardToken>(map->operator[](name).get());

      return *this;
    }

    template <class... Args>
      requires(std::formattable<Args, char> && ...)
    Out &operator+(Args &&...args) {
      (this->AddFormatted("{}", args) && ...);
      return *this;
    }

    template <class... Args> Out &operator=(uint32_t count) {
      for (size_t i = 0; i < count; i++) {
        appendLine("\n");
      }
      return *this;
    }

    template <class... Args>
      requires(std::formattable<Args, char> && ...)
    void AddFormatted(std::format_string<Args...> fmt, Args &&...args) {
      this->appendLine(std::format(fmt, std::forward<Args>(args)...));
    }

    // Out &operator-(std::string_view &&wildcard) {
    //   flushLine();
    //   *(this->it) = std::make_unique<WildcardToken>(
    //       this->parent.leftPorts[wildcard].get());
    //   return *this;
    // }

    // auto AddLeftPort(Module &self, std::string_view key) -> absl::Status {
    //   if (self.leftPorts.contains(key))
    //     return absl::NotFoundError(std::format("Port '{}' not found!", key));
    //   *this->it = std::make_unique<WildcardToken>(self.leftPorts[key].get());
    //   return absl::OkStatus();
    // }
  };

  // Generates CodegenTokens
  virtual auto GenerateTokenString(ContextProvider *context, Out out)
      -> absl::Status = 0;
};
inline auto operator/(Module::Out::Polarity pol, std::string_view &&name)
    -> Module::Out::PortFetch {
  return {pol, name};
}

/**
 * @brief Graph Node
 */
class Graph : public Node {
private:
  std::vector<std::unique_ptr<Node>> subnodes;

public:
};

/**
 * @brief Graph Connection
 */
typedef struct Connection : public Identifiable<Connection> {
  // Left and right port of the connection
  Port *left_port;
  Port *right_port;
} Connection;
} // namespace msk::ir
