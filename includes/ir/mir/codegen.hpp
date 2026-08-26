#include <cstdint>
#include <memory>
#include <string>
#pragma ONCE

namespace msk::ir {
struct Connection; // pimpl - definition in src

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
  std::unique_ptr<Connection> connection;

public:
  std::string GetString();
} WildcardToken;
} // namespace msk::ir
