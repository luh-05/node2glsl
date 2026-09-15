#pragma once
#include <string>
#include <pugixml.hpp>
#include "blender/ir_shim/ir_shim.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>

namespace msk::blender {

class XMLParser {
    pugi::xml_parse_result parse_result;

public:
 auto XMLread(std::string_view path) -> absl::Status;
 auto ParseGraph(std::string_view target_graph_id) -> absl::StatusOr<const GraphHandle>;
};

}