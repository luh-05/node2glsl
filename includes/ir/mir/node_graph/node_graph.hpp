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

    Out(Inserter &&it) : it(it) {};

    void Add(std::string s) { *(this->it) = std::make_unique<TextToken>(s); }

    template <class... Args>
    void AddF(std::format_string<Args...> fmt, Args &&...args) {
      this->Add(std::format(fmt, args...));
    }

    void AddLine(std::string s) { this->AddF("{}\n", s); }
    void AddLine() { this->AddLine(""); }

    template <class... Args>
      requires(std::formattable<Args, char> && ...)
    void AddLineF(std::format_string<Args...> fmt, Args &&...args) {
      auto s = std::format(fmt, args...);
      this->Add(std::format("{}\n", s));
    }

    template <class... Args>
    // requires(std::same_as<std::string, Args> && ...)
    void AddLines(std::string &&s, Args &&...strings) {
      this->AddLine(s);
      AddLines(strings...);
    }
    void AddLines(std::string &&s) { this->AddLine(s); }

    class FormattingLine {
    public:
      template <class... Args>
        requires(std::formattable<Args, char> && ...)
      FormattingLine(bool linebreak, std::format_string<Args...> fmt,
                     Args &&...args)
          : invoke_([fmt, ... args = args, linebreak](Out &self) mutable {
              if (linebreak)
                self.AddLineF<Args...>(fmt, args...);
              else
                self.AddF<Args...>(fmt, args...);
            }) {}
      template <class... Args>
        requires(std::formattable<Args, char> && ...)
      FormattingLine(std::format_string<Args...> fmt, Args &&...args)
          : invoke_([fmt, ... args = args](Out &self) mutable {
              self.AddLineF<Args...>(fmt, args...);
            }) {}

      void operator()(Out &self) const { invoke_(self); }

    private:
      std::function<void(Out &)> invoke_;
    };
    void AddLinesF(std::initializer_list<FormattingLine> lines) {
      for (auto &l : lines)
        l(*this);
    }

    auto AddLeftPort(Module &self, std::string_view key) -> absl::Status {
      if (self.leftPorts.contains(key))
        return absl::NotFoundError(std::format("Port '{}' not found!", key));
      *this->it = std::make_unique<WildcardToken>(self.leftPorts[key].get());
      return absl::OkStatus();
    }
    auto AddRightPort(Module &self, std::string_view key) -> absl::Status {
      if (self.rightPorts.contains(key))
        return absl::NotFoundError(std::format("Port '{}' not found!", key));
      *this->it = std::make_unique<WildcardToken>(self.rightPorts[key].get());
      return absl::OkStatus();
    }
  };

  // Generates CodegenTokens
  virtual auto GenerateTokenString(ContextProvider *context, Out out)
      -> absl::Status = 0;
};

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
