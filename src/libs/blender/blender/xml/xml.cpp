#include "blender/xml/xml.hpp"
#include "blender/ir_shim/ir_shim.hpp"
#include <pugixml.hpp>
#include <format>

namespace msk::blender {


auto XMLParser::XMLread(std::string_view path) -> absl::Status {

    pugi::xml_document doc;
    this -> parse_result = doc.load_file(std::string(path).c_str());

    if(!parse_result) {
        return absl::NotFoundError(std::format("Could not load file: {} XML-Error: {}", path, parse_result.description()));
    }

    return absl::OkStatus();
}


}