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
};

class GraphHandle {
  friend GraphShim;

private:
  ir::Graph *graph;
};

class PortHandle {
  friend GraphShim;

private:
  ir::Port *port;
};

class GraphShim {
private:
  std::unique_ptr<ir::Graph> graph;

  auto CreateGraph() -> std::unique_ptr<ir::Graph>;

public:
  GraphShim();

  auto AddModule(const GraphHandle graph, std::string_view name,
                 std::string_view type) -> absl::StatusOr<const ModuleHandle>;
  auto AddSubGraph(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const GraphHandle>;
  auto AddPort(const ModuleHandle module, std::string_view name,
               std::string_view datatype) -> absl::StatusOr<const PortHandle>;
  auto AddPort(const GraphHandle graph, std::string_view name,
               std::string_view datatype) -> absl::StatusOr<const PortHandle>;

  auto GetGraph() -> GraphHandle;
  auto GetModule(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const ModuleHandle>;
  auto GetSubGraph(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const GraphHandle>;
  auto GetPort(const ModuleHandle module, std::string_view name)
      -> absl::StatusOr<const PortHandle>;
  auto GetPort(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const PortHandle>;
};

} // namespace msk::blender
