#include <absl/status/statusor.h>
#include <string>
#include <variant>
#pragma once

namespace msk::ir {
struct Port; // pimpl - definition in node_graph.hpp

/**
 * @brief Abstract Token basetype for codegen
 */
// class CodegenToken {
//
// public:
//   virtual std::string GetString() = 0;
//   virtual ~CodegenToken() {};
// };

/**
 * @brief CodegenToken specification for text
 */
class TextToken {
private:
  std::string text;

public:
  TextToken(std::string text) : text(text) {}
  ~TextToken() {}
  std::string GetString();
};

/**
 * @brief CodegenToken specification for a connection
 */
class WildcardToken {
private:
  Port *port;

public:
  WildcardToken(Port *port) : port(port) {};
  ~WildcardToken() {}
  auto GetString() -> absl::StatusOr<std::string>;
};

// using CodegenToken = std::variant<TextToken, WildcardToken>;
} // namespace msk::ir
