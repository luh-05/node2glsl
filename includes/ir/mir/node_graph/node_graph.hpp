#pragma once
// #include "mir/codegen.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <cstdint>
#include <format>
#include <iterator>
#include <map>
#include <memory>
#include <mir/utils.hpp>
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
  auto GetConstant(Node *n, std::string_view name) -> absl::StatusOr<T>;
};

/**
 * @brief Specification of Node for Modules
 */
class Module : public Node {
private:
  std::shared_ptr<ContextProvider> cxt;

public:
  ContextProvider *GetContext() { return cxt.get(); };

  Module(std::shared_ptr<ContextProvider> cxt) : cxt(cxt) {}

  using Token = std::unique_ptr<CodegenToken>;

  /**
   *  @brief Helper Class for specifying Module::GenerateTokenString(), provides
   * a mini DSL
   */
  class Out {
  private:
    absl::Status status = absl::OkStatus();
    std::string text_buff;

  public:
    using Inserter = std::back_insert_iterator<std::vector<Token>>;
    Inserter it;
    Module &parent;

    Out(Inserter it, Module &parent) : it(it), parent(parent) {};
    ~Out() noexcept {
      try {
        if (!text_buff.empty())
          FlushBuffer();
      } catch (...) {
      }
    }

    /**
     *  @brief Gets the stored status, supposed to be used as the return value
     * for Module::GenerateTokenString()
     */
    [[nodiscard]] auto GetStatus() -> absl::Status { return this->status; }

    /**
     *  @brief Gets a Constant from the GraphContext
     *  @throw When a constant is not found, the internal status will be set and
     * no further codegen will be possible from this object
     */
    template <class T> const T GetConstant(std::string_view name) {
      if (auto s = parent.cxt->GetConstant<const T>(&parent, name); !s.ok()) {
        if (this->status.ok())
          this->status = s.status();
        return 0;
      } else
        return s.value();
    }

    // Specifies port polarity
    enum Polarity { LEFT = 0x0, RIGHT = 0x1 };
    using PortFetch = std::tuple<Polarity, std::string_view>;

    /**
     *  @brief Helper for appending something to the buffer
     */
    template <class T> inline void appendToBuffer(T s) {
      if (status == absl::OkStatus())
        this->text_buff += s;
    }

    /**
     *  @brief Flushes the buffer into a TextToken
     */
    void FlushBuffer();

    /**
     *  @brief Creates a WildcardToken - automatically flushes
     */
    Out &operator+(PortFetch fetch);

    /**
     *  @brief Adds someting to the current buffer
     */
    template <class T>
      requires(std::formattable<T, char>)
    inline Out &operator+(T &&arg) {
      this->AddFormatted("{}", std::forward<T>(arg));
      return *this;
    }

    /**
     *  @brief Appends the specified amount of newlines, also ends statement due
     * to operator precedence
     */
    inline Out &operator=(uint32_t count) {
      for (size_t i = 0; i < count; i++) {
        appendToBuffer("\n");
      }
      return *this;
    }

    /**
     *  @brief Appends a formatted string to the buffer
     */
    template <class... Args>
      requires(std::formattable<Args, char> && ...)
    inline void AddFormatted(std::format_string<Args...> fmt, Args &&...args) {
      this->appendToBuffer(std::format(fmt, std::forward<Args>(args)...));
    }
  };

  // Generates CodegenTokens
  virtual auto GenerateTokenString(Out &&out) -> absl::Status = 0;
};
inline auto operator/(Module::Out::Polarity pol, std::string_view name)
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
