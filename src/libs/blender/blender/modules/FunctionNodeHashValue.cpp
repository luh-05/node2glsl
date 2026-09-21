#include "mir/node_graph/node_graph.hpp"
#include "modules.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>
#include <string>

namespace msk::blender {

auto GenerateTokenStringFunctionNodeHashValue(Out &out) -> absl::Status {
  auto data_type = out.GetConstant<std::string>("data_type0");

  // FIXME: Implement GLSL Helper Function
  // GLSL does not have hash function?

  return absl::UnimplementedError("Function has not been implemented yet.");
}

} // namespace msk::blender
