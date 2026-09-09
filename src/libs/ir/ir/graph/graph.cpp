#pragma once

#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <memory>
#include <string_view>
namespace msk::ir {

// --- PORT ---

auto Port::EstablishConnection(Port &other) -> absl::Status {
  if (this->connection) {
    auto status = absl::AlreadyExistsError("Port already has a connection!");
    status.SetPayload("mollusk.ir/AlreadyExistsReason", absl::Cord("this"));

    return status;
  }
  if (other.connection) {
    auto status = absl::AlreadyExistsError("Port already has a connection!");
    status.SetPayload("mollusk.ir/AlreadyExistsReason", absl::Cord("other"));

    return status;
  }

  if (this->dataType != other.dataType) {
    return absl::InvalidArgumentError(
        "Cannot establish connection between ports of differing datatypes!");
  }

  this->connection = std::make_shared<Connection>(this, &other);
  other.connection = this->connection;

  return absl::OkStatus();
}

// --- NODE ---

auto addPort(Node::MapType &map, std::string_view name,
             std::string_view data_type) -> absl::Status {
  if (map.contains(name)) {
    return absl::AlreadyExistsError("Port already exists!");
  }

  map[std::string(name)] = std::make_unique<Port>(std::string(data_type));
  return absl::OkStatus();
}

auto Node::AddLeftPort(std::string_view name, std::string_view data_type)
    -> absl::StatusOr<Port *> {
  auto status = addPort(this->leftPorts, name, data_type);

  if (!status.ok()) {
    return status;
  }

  return this->GetLeftPort(name);
}
auto Node::AddRightPort(std::string_view name, std::string_view data_type)
    -> absl::StatusOr<Port *> {
  auto status = addPort(this->rightPorts, name, data_type);

  if (!status.ok()) {
    return status;
  }

  return this->GetRightPort(name);
}

auto getPort(Node::MapType &map, std::string_view name)
    -> absl::StatusOr<Port *> {
  if (!map.contains(name)) {
    return absl::NotFoundError("Port not found!");
  }

  return map.at(std::string(name)).get();
}

auto Node::GetLeftPort(std::string_view name) -> absl::StatusOr<Port *> {
  return getPort(this->leftPorts, name);
}
auto Node::GetRightPort(std::string_view name) -> absl::StatusOr<Port *> {
  return getPort(this->rightPorts, name);
}

// --- GRAPH ---

template <class T>
auto Graph::addNode(std::string_view name) -> absl::StatusOr<T *> {
  if (this->subnodes.contains(name)) {
    return absl::AlreadyExistsError("Graph already exists!");
  }

  this->subnodes[std::string(name)] = std::make_unique<T>();
  // return absl::OkStatus();

  auto node_status = this->GetNode<Module>(name);

  if (!node_status.ok()) {
    return node_status.status();
  }

  return node_status.value();
}

template <class T>
auto Graph::GetNode(std::string_view name) -> absl::StatusOr<T *> {
  if (!this->subnodes.contains(name)) {
    return absl::NotFoundError("Node not found!");
  }

  if (auto *node = std::get_if<std::unique_ptr<T>>(
          &this->subnodes.at(std::string(name)))) {
    return node->get();
  }

  return absl::InvalidArgumentError("Node found but type doesn't match!");
}

} // namespace msk::ir
