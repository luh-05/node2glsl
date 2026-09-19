#include "blender/modules/modules.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <array>
#include <memory>
#include <mir/codegen.hpp>

namespace msk::blender {

auto FunctionNodeInputVector::GenerateTokenString(Out &&out) -> absl::Status {
  auto vector_var = out.GetConstant<std::string>("vector_dimensions0");

  out + Out::RIGHT / "Vector0" + vector_var + ";";

  return out.GetStatus();
}

} // namespace msk::blender
