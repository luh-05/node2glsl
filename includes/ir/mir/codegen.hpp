#include <memory>
#include <string>
#pragma ONCE

namespace msk::ir {
struct Port; // pimpl - definition in node_graph.hpp

/**
 * @brief Abstract Token basetype for codegen
 */
typedef class CodegenToken {
private:
  virtual std::string GetString() = 0;
} CodegenToken;

/**
 * @brief CodegenToken specification for text
 */
typedef class TextToken : CodegenToken {
private:
  std::string text;

public:
  TextToken(std::string text) : text(text) {}
  std::string GetString();
} TextToken;

/**
 * @brief CodegenToken specification for a connection
 */
typedef class WildcardToken : CodegenToken {
private:
  std::weak_ptr<Port> port;

public:
  WildcardToken(std::weak_ptr<Port> port) : port(port) {};
  std::string GetString();
} WildcardToken;
} // namespace msk::ir
