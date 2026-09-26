#pragma once

#include <absl/status/statusor.h>
#include <memory>
#include <mir/node_graph/GraphContext.hpp>

namespace msk::blender {

// TODO: Move this over into core library and change parsing to use HostGraphAPI
// and HostConstantAPI
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
  std::shared_ptr<ir::GraphContext> context;

public:
  GraphShim() { this->context = std::make_shared<ir::GraphContext>(); };

  enum Polarity { LEFT, RIGHT };

  /// Add module
  auto AddModule(const GraphHandle graph, std::string_view name,
                 ModuleFunc impl) -> absl::StatusOr<const ModuleHandle>;
  /// Add subgraph
  auto AddSubGraph(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const GraphHandle>;
  /// Add port to module
  auto AddPort(const ModuleHandle module, Polarity p, std::string_view name,
               std::string_view datatype) -> absl::StatusOr<const PortHandle>;
  /// Add port to graph
  auto AddPort(const GraphHandle graph, Polarity p, std::string_view name,
               std::string_view datatype) -> absl::StatusOr<const PortHandle>;
  /// Connect two ports
  auto ConnectPorts(const PortHandle left, const PortHandle right)
      -> absl::Status;

  /// Add constant to node
  auto AddConstant(const ModuleHandle module, std::string_view name,
                   std::string value) -> absl::Status;

  /// Get main graph
  inline auto GetGraph() -> GraphHandle {
    return GraphHandle(this->context->graph.get());
  }
  /// Get GraphContext
  inline auto GetSharedPointer() -> std::shared_ptr<ir::GraphContext> {
    return this->context;
  }
  /// Get module
  auto GetModule(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const ModuleHandle>;
  /// Get subgraph
  auto GetSubGraph(const GraphHandle graph, std::string_view name)
      -> absl::StatusOr<const GraphHandle>;
  /// Get port form module
  auto GetPort(const ModuleHandle module, Polarity p, std::string_view name)
      -> absl::StatusOr<const PortHandle>;
  /// Get port from graph
  auto GetPort(const GraphHandle graph, Polarity p, std::string_view name)
      -> absl::StatusOr<const PortHandle>;
};

} // namespace msk::blender
