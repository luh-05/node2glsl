#include <absl/status/statusor.h>
#include <cstdint>
#include <mir/codegen.hpp>
#include <string>
#pragma ONCE

namespace msk::ir {

class ConnectionResolver {
private:
  uint32_t next_id;

public:
  auto resolveConnection(WildcardToken token) -> absl::StatusOr<std::string>;
};
} // namespace msk::ir
