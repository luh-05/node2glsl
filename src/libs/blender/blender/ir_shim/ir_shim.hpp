#pragma once

#include <absl/status/statusor.h>
#include <memory>
#include <mir/node_graph/node_graph.hpp>

namespace msk::blender {

class GraphShim;

class ModuleHandle {
  friend GraphShim;

private:
  ir::Module *module;

public:
  ModuleHandle(ir::Module *module) : module(module) {};
};

class GraphHandle {
  friend GraphShim;

private:
  ir::Graph *graph;

public:
  GraphHandle(ir::Graph *graph) : graph(graph) {};
};

class PortHandle {
  friend GraphShim;

private:
  ir::Port *port;

public:
  PortHandle(ir::Port *port) : port(port) {};
};

class GraphShim {
private:
  std::unique_ptr<ir::Graph> graph;

public:
  GraphShim();

  enum Polarity { LEFT, RIGHT };

  auto AddModule(const GraphHandle graph, std::string_view name,
                 std::string_view type) -> absl::StatusOr<const ModuleHandle>;
  auto AddSubGraph(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const GraphHandle>;
  auto AddPort(const ModuleHandle module, Polarity p, std::string_view name,
               std::string_view datatype) -> absl::StatusOr<const PortHandle>;
  auto AddPort(const GraphHandle graph, Polarity p, std::string_view name,
               std::string_view datatype) -> absl::StatusOr<const PortHandle>;
  auto ConnectPorts(const PortHandle left, const PortHandle right)
      -> absl::Status;

  inline auto GetGraph() -> GraphHandle {
    return GraphHandle(this->graph.get());
  }
  auto GetModule(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const ModuleHandle>;
  auto GetSubGraph(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const GraphHandle>;
  auto GetPort(const ModuleHandle module, Polarity p, std::string_view name)
      -> absl::StatusOr<const PortHandle>;
  auto GetPort(const GraphHandle graph, Polarity p, std::string_view name)
      -> absl::StatusOr<const PortHandle>;
};

} // namespace msk::blender
