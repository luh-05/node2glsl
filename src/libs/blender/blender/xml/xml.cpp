#include "blender/xml/xml.hpp"
#include "blender/ir_shim/ir_shim.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <format>
#include <memory>
#include <pugixml.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

namespace msk::blender {

auto XMLParser::XMLread(std::string_view path) -> absl::Status {

  pugi::xml_parse_result result = doc.load_file(std::string(path).c_str());

  if (!result) {
    return absl::NotFoundError(std::format(
        "Could not load file: {} XML-Error: {}", path, result.description()));
  }

  return absl::OkStatus();
}

auto XMLParser::ParseGraph(std::string_view target_graph_id,
                           std::shared_ptr<msk::ir::GraphContext> graph_context)
    -> absl::StatusOr<std::shared_ptr<ir::Graph>> {

  std::string xpath_query = std::format("//Graph[@id='{}']", target_graph_id);
  pugi::xpath_node xpath_graph = doc.select_node(xpath_query.c_str());

  if (!xpath_graph) {
    return absl::NotFoundError(
        std::format("Could not find graph: id {}", target_graph_id));
  }

  pugi::xml_node xml_graph = xpath_graph.node();

  GraphShim graph(graph_context);
  GraphHandle graph_hande = graph.GetGraph();
  std::unordered_map<std::string, PortHandle> port_id_map;

  absl::Status populate_graph_status =
      PopulateGraph(graph, graph_hande, xml_graph, port_id_map);
  if (!populate_graph_status.ok())
    return populate_graph_status;

  return graph.GetSharedPointer();
}

auto XMLParser::PopulateGraph(
    GraphShim &graph, GraphHandle current_graph, pugi::xml_node xml_graph,
    std::unordered_map<std::string, PortHandle> &port_id_map) -> absl::Status {

  // nodes
  for (pugi::xml_node xml_node : xml_graph.children("Node")) {
    std::string node_name = xml_node.attribute("name").value();
    std::string node_type = xml_node.attribute("type").value();

    auto module_or = graph.AddModule(current_graph, node_name, node_type);
    if (!module_or.ok())
      return module_or.status();
    ModuleHandle module = *module_or;

    // ports
    for (pugi::xml_node xml_port : xml_node.children("Port")) {
      std::string port_name = xml_port.attribute("name").value();
      std::string direction = xml_port.attribute("direction").value();
      std::string port_id = xml_port.attribute("id").value();

      if (direction != "in" && direction != "out") {
        return absl::InvalidArgumentError(
            std::format("Bad XML; Port direction is invalid, should be 'in' or "
                        "'out', is: {}. Node: {}, Port[name: {}, id: {}]",
                        direction, node_name, port_name, port_id));
      }
      GraphShim::Polarity port_polarity = (direction == "in")
                                              ? GraphShim::Polarity::LEFT
                                              : GraphShim::Polarity::RIGHT;

      auto port_or = graph.AddPort(module, port_polarity, port_name, "");
      if (!port_or.ok())
        return port_or.status();

      if (port_id.empty())
        return absl::InvalidArgumentError(
            std::format("Bad XML; Port ID is empty. Node: {}, Port: {}",
                        node_name, port_name));

      if (port_id_map.contains(port_id))
        return absl::InvalidArgumentError(std::format(
            "Bad XML; Port ID is duplicate. Node: {}, Port[name: {}, id: {}]",
            node_name, port_name, port_id));

      port_id_map.emplace(port_id, *port_or);
    }

    // constants
    for (pugi::xml_node xml_constant : xml_node.children("Constant")) {
      std::string constant_name = xml_constant.attribute("name").value();
      std::string constant_value = xml_constant.attribute("value").value();

      if (auto constant_or =
              graph.AddConstant(module, constant_name, constant_value);
          !constant_or.ok())
        return constant_or;
    }
  }

  // subgraphs (recursive)
  for (pugi::xml_node xml_subgraph : xml_graph.children("Graph")) {
    std::string subgraph_name = xml_subgraph.attribute("name").value();

    auto subgraph_or = graph.AddSubGraph(current_graph, subgraph_name);
    if (!subgraph_or.ok())
      return subgraph_or.status();
    GraphHandle subgraph = *subgraph_or;

    absl::Status populate_subgraph_status =
        PopulateGraph(graph, subgraph, xml_subgraph, port_id_map);
    if (!populate_subgraph_status.ok())
      return populate_subgraph_status;
  }

  // connects
  for (pugi::xml_node xml_connection : xml_graph.children("Connection")) {

    std::string from_id = xml_connection.attribute("from").value();
    std::string to_id = xml_connection.attribute("to").value();

    auto from_port_handle = port_id_map.find(from_id);
    auto to_port_handle = port_id_map.find(to_id);

    if (from_port_handle == port_id_map.end() ||
        to_port_handle == port_id_map.end()) {
      return absl::NotFoundError(
          std::format("Failed to connect ports: At least one of the Port IDs "
                      "not found (from_id: {}; to_id {})",
                      from_id, to_id));
    }

    absl::Status connection_status =
        graph.ConnectPorts(from_port_handle->second, to_port_handle->second);
    if (!connection_status.ok())
      return connection_status;
  }

  return absl::OkStatus();
}

} // namespace msk::blender
