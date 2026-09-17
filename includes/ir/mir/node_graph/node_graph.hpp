#pragma once
// #include "mir/codegen.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <cstdint>
#include <format>
#include <initializer_list>
#include <iterator>
#include <map>
#include <memory>
#include <mir/utils.hpp>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace msk::ir {
class Connection;
class Port : public Identifiable<Port> {
public:
  // TODO: change to enum
  std::string dataType; // Data type of Port

  // std::vector<std::shared_ptr<Connection>> connections;
  using ConnectionPointer = std::shared_ptr<Connection>;
  std::variant<ConnectionPointer, std::vector<ConnectionPointer>> connection;

  Port(std::string dataType, bool left) : dataType(dataType) {
    if (!left) {
      connection = std::vector<ConnectionPointer>();
    }
  };

  /**
   * @brief Establishes connection with another port
   *
   * @return absl::AlreadyExistsError - payload url:
   * "mollusk.ir/AlreadyExistsReason" ("this" meaning this port already has a
   * connection and "other for the other")
   * @return absl::InvalidArgumentError when port datatypes are incompatible
   * @return absl::OkStatus() if success
   */
  auto EstablishConnection(Port &other) -> absl::Status;
};

class CodegenToken; // pimpl
/**
 * @brief Node representation in Graph
 */
class Node : public Identifiable<Node> {
public:
  using MapType = std::map<std::string, std::unique_ptr<Port>, std::less<>>;
  // Left and right ports, maps name to port
  MapType leftPorts;
  MapType rightPorts;

  auto AddLeftPort(std::string_view name, std::string_view dataType)
      -> absl::StatusOr<Port *>;
  auto AddRightPort(std::string_view name, std::string_view dataType)
      -> absl::StatusOr<Port *>;

  auto GetLeftPort(std::string_view name) -> absl::StatusOr<Port *>;
  auto GetRightPort(std::string_view name) -> absl::StatusOr<Port *>;
};

class GraphContext; // pimpl
/**
 * @brief Connector between internal GraphContext and interface for Module
 * specification
 */
class ContextProvider {
private:
  std::shared_ptr<GraphContext> context;

public:
  ContextProvider(std::shared_ptr<GraphContext> context);

  // Gets the named constant of the provided node
  template <class T>
  auto GetConstant(Node *n, std::string_view name) -> absl::StatusOr<T>;
};

/**
 * @brief Specification of Node for Modules
 */
class Module : public Node {
public:
  std::string type;

  using Token = std::unique_ptr<CodegenToken>;

  Module(std::string type) : type(type) {}

  /**
   *  @brief Helper Class for specifying Module::GenerateTokenString(), provides
   * a mini DSL
   */
  class Out {
  public:
    class Legacy;
    friend Legacy;

    // Specifies port polarity
    enum Polarity { LEFT = 0x0, RIGHT = 0x1 };

  private:
    std::shared_ptr<ContextProvider> cxt;

    ContextProvider *GetContext() { return cxt.get(); };

    absl::Status status = absl::OkStatus();
    std::string text_buff;

    bool checkPort(Polarity p, std::string name, Node::MapType *&map);
    auto createWildcardToken(std::string_view name, Node::MapType *&map)
        -> std::unique_ptr<CodegenToken>;

  public:
    using Inserter = std::back_insert_iterator<std::vector<Token>>;
    Inserter it;
    Module &parent;
    std::unique_ptr<Legacy> legacy;

    Out(std::shared_ptr<ContextProvider> cxt, Inserter it, Module &parent)
        : cxt(cxt), it(it), parent(parent) {
      this->legacy = std::make_unique<Legacy>(*this);
    };
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
    auto GetStatus() -> absl::Status { return this->status; }

    /**
     *  @brief Gets a Constant from the GraphContext
     *  @throw When a constant is not found, the internal status will be set and
     * no further codegen will be possible from this object
     */
    template <class T> auto GetConstant(std::string_view name) -> T {
      if (auto s = cxt->GetConstant<T>(static_cast<Node *>(&parent), name);
          !s.ok()) {
        if (this->status.ok())
          this->status = s.status();
        return {};
      } else
        return s.value();
    }

    // Fetch tuple
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

    // Legacy compatibility API - usage NOT recommended
    // Use overload DSL instead
    class Legacy {
    private:
      Out &self;

      class MockToken {
      public:
        virtual ~MockToken() = default;
      };
      class MTextToken : public MockToken {
      public:
        std::string text;
        MTextToken(std::string text) : text(text) {}
      };
      class MWildcardToken : public MockToken {
      public:
        Polarity p;
        std::string name;
        MWildcardToken(Polarity p, std::string name) : p(p), name(name) {}
      };

    public:
      Legacy(Out &self) : self(self) {}

      /**
       * @brief Convert MockTokens to real Tokens
       */
      void AddTokenVector(std::vector<std::shared_ptr<MockToken>> &&s);
      /**
       * @brief Create MTextToken
       */
      auto CreateMTT(std::string text) -> std::shared_ptr<MTextToken> {
        return std::make_shared<MTextToken>(text);
      }
      /**
       * @brief Create MWildcardToken
       */
      auto CreateMWT(Polarity p, std::string name)
          -> std::shared_ptr<MWildcardToken> {
        return std::make_shared<MWildcardToken>(p, name);
      }
    };
  };

  // Generates CodegenTokens
  typedef absl::Status (*GenerateTokenString)(Out &&out);
  GenerateTokenString impl;
  // virtual auto GenerateTokenString(Out &&out) -> absl::Status {
  //   return absl::NotFoundError("GenerateTokenString not implemented!");
  // }
};
inline auto operator/(Module::Out::Polarity pol, std::string_view name)
    -> Module::Out::PortFetch {
  return {pol, name};
}

/**
 * @brief Graph Node
 */
class Graph : public Node {
public:
  using VariantType =
      std::variant<std::unique_ptr<Module>, std::unique_ptr<Graph>>;
  using MapType = std::map<std::string, VariantType, std::less<>>;

private:
  MapType subnodes;

  template <class T, class... Args>
  auto addNode(std::string_view name, Args... args) -> absl::StatusOr<T *>;

public:
  auto AddModule(std::string_view name, std::string_view type)
      -> absl::StatusOr<Module *>;
  auto AddGraph(std::string_view name) -> absl::StatusOr<Graph *>;
  template <class T> auto GetNode(std::string_view name) -> absl::StatusOr<T *>;
};

/**
 * @brief Graph Connection
 */
class Connection : public Identifiable<Connection> {
public:
  // Left and right port of the connection
  Port *left_port;
  Port *right_port;

  Connection(Port *left_port, Port *right_port)
      : left_port(left_port), right_port(right_port) {}
};
} // namespace msk::ir
