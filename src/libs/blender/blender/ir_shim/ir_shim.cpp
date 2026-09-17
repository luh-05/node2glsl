#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <blender/ir_shim/ir_shim.hpp>
#include <ir/graph/graph.hpp>
#include <memory>

namespace msk::blender {
GraphShim::GraphShim(std::shared_ptr<ir::GraphContext> context) {
  this->graph = std::make_shared<ir::Graph>();
  this->context = context;
}

auto GraphShim::AddModule(const GraphHandle graph, std::string_view name,
                          ir::Module::GenerateTokenString type)
    -> absl::StatusOr<const ModuleHandle> {
  auto status = graph.graph->AddModule(name, type);

  if (!status.ok()) {
    return absl::AbortedError(std::format("Failed to add module '{}': {}", name,
                                          status.status().ToString()));
  }

  return ModuleHandle(status.value());
}

auto GraphShim::AddSubGraph(const GraphHandle graph, std::string_view name)
    -> absl::StatusOr<const GraphHandle> {
  auto status = graph.graph->AddGraph(name);

  if (!status.ok()) {
    return absl::AbortedError(std::format("Failed to add graph '{}': {}", name,
                                          status.status().ToString()));
  }

  return GraphHandle(status.value());
}

template <class R, class T>
auto getNode(ir::Graph &m, std::string_view name, std::string err_msg)
    -> absl::StatusOr<const R> {

  auto status = m.GetNode<T>(name);

  if (!status.ok()) {
    return absl::AbortedError(
        std::format("{}: {}", err_msg, status.status().ToString()));
  }

  return status.value();
}

auto GraphShim::GetModule(const GraphHandle graph, std::string_view name)
    -> absl::StatusOr<const ModuleHandle> {
  return getNode<ModuleHandle, ir::Module>(
      *graph.graph, name, std::format("Failed to get module '{}'", name));
}
auto GraphShim::GetSubGraph(const GraphHandle graph, std::string_view name)
    -> absl::StatusOr<const GraphHandle> {
  return getNode<GraphHandle, ir::Graph>(
      *graph.graph, name, std::format("Failed to get graph '{}'", name));
}

template <class T>
auto addPort(T &m, GraphShim::Polarity p, std::string_view name,
             std::string_view data_type) -> absl::StatusOr<const PortHandle> {
  absl::StatusOr<ir::Port *> status;
  if (p == GraphShim::LEFT) {
    status = m.AddLeftPort(name, data_type);
  } else if (p == GraphShim::RIGHT) {
    status = m.AddRightPort(name, data_type);
  } else {
    return absl::InvalidArgumentError(
        "Polarity must be GraphShim::Polarity::LEFT or "
        "GraphShim::Polarity::RIGHT");
  }

  if (!status.ok()) {
    return absl::AbortedError(
        std::format("Failed to add {} port '{}' of type '{}': {}",
                    p == GraphShim::LEFT ? "left" : "right", name, data_type,
                    status.status().ToString()));
  }

  return PortHandle(status.value());
}

auto GraphShim::AddPort(const ModuleHandle module, Polarity p,
                        std::string_view name, std::string_view data_type)
    -> absl::StatusOr<const PortHandle> {
  return addPort(*module.module, p, name, data_type);
}
auto GraphShim::AddPort(const GraphHandle graph, Polarity p,
                        std::string_view name, std::string_view data_type)
    -> absl::StatusOr<const PortHandle> {
  return addPort(*graph.graph, p, name, data_type);
}

auto GraphShim::ConnectPorts(const PortHandle right, const PortHandle left)
    -> absl::Status {
  auto status = right.port->EstablishConnection(*left.port);

  if (status.ok())
    return absl::OkStatus();
  if (status.code() == absl::StatusCode::kAlreadyExists) {
    auto payload = status.GetPayload("mollusk.ir/AlreadyExistsReason");

    if (!payload.has_value()) {
      return absl::AbortedError(
          std::format("A port alredy has a connection, but it could not be "
                      "detemined which: {}",
                      status.ToString()));
    }

    auto payload_s = payload.value().Flatten();
    return absl::AbortedError(
        std::format("{} port already has a connection: {}",
                    payload_s == "this"    ? "left"
                    : payload_s == "other" ? "right"
                                           : "UNKNOWN",
                    status.ToString()));
  } else if (status.code() == absl::StatusCode::kInvalidArgument) {
    return absl::AbortedError(std::format(
        "Port types do not match! (right: '{}', left: '{}'): {}",
        right.port->dataType, left.port->dataType, status.ToString()));
  }

  return absl::AbortedError(std::format(
      "Unhandled error in connecting ports: {}", status.ToString()));
}

template <class T>
auto getPort(T &m, GraphShim::Polarity p, std::string_view name)
    -> absl::StatusOr<const PortHandle> {
  absl::StatusOr<ir::Port *> status;
  if (p == GraphShim::LEFT) {
    status = m.GetLeftPort(name);
  } else if (p == GraphShim::RIGHT) {
    status = m.GetRightPort(name);
  } else {
    return absl::InvalidArgumentError(
        "Polarity must be GraphShim::Polarity::LEFT or "
        "GraphShim::Polarity::RIGHT");
  }

  if (!status.ok()) {
    return absl::AbortedError(
        std::format("Failed to get {} port '{}': {}",
                    p == GraphShim::LEFT ? "left" : "right", name,
                    status.status().ToString()));
  }

  return PortHandle(status.value());
}

auto GraphShim::GetPort(const ModuleHandle module, Polarity p,
                        std::string_view name)
    -> absl::StatusOr<const PortHandle> {
  return getPort(*module.module, p, name);
}

auto GraphShim::GetPort(const GraphHandle graph, Polarity p,
                        std::string_view name)
    -> absl::StatusOr<const PortHandle> {
  return getPort(*graph.graph, p, name);
}

auto GraphShim::AddConstant(const ModuleHandle module, std::string_view name,
                            std::string value) -> absl::Status {
  return this->context.get()->AddConstant(module.module, name, value);
}

} // namespace msk::blender
