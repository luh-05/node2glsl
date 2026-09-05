#include <string>
#pragma once

namespace msk::ir {
struct Port; // pimpl - definition in node_graph.hpp

/**
 * @brief Abstract Token basetype for codegen
 */
typedef class CodegenToken {

public:
  virtual std::string GetString() = 0;
  virtual ~CodegenToken() {};
} CodegenToken;

/**
 * @brief CodegenToken specification for text
 */
typedef class TextToken : public CodegenToken {
private:
  std::string text;

public:
  TextToken(std::string text) : text(text) {}
  ~TextToken() {}
  std::string GetString();
} TextToken;

/**
 * @brief CodegenToken specification for a connection
 */
typedef class WildcardToken : public CodegenToken {
private:
  Port *port;

public:
  WildcardToken(Port *port) : port(port) {};
  ~WildcardToken() {}
  std::string GetString();
} WildcardToken;
} // namespace msk::ir
