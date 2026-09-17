#pragma once
#include "blender/ir_shim/ir_shim.hpp"
#include "mir/node_graph/GraphContext.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <map>
#include <memory>
#include <pugixml.hpp>
#include <string>

namespace msk::blender {

class XMLParser {
  pugi::xml_document doc;

private:
  auto PopulateGraph(GraphShim &graph, GraphHandle current_graph,
                     pugi::xml_node xml_graph,
                     std::unordered_map<std::string, PortHandle> &port_id_map)
      -> absl::Status;

public:
  auto XMLread(std::string_view path) -> absl::Status;
  auto ParseGraph(std::string_view target_graph_id)
      -> absl::StatusOr<std::shared_ptr<ir::GraphContext>>;
};

} // namespace msk::blender
