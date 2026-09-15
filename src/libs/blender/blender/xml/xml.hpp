#pragma once
#include <string>

namespace msk::blender {

class XMLParser {
    pugi::xml_parse_result parse_result;

public:
 auto XMLread(std::string_view path) -> absl::Status;
 auto ParseGraph(std::string_view target_graph_id) -> absl::StatusOr<const GraphHandle>;
};

}