#include "mir/node_graph/node_graph.hpp"
#include "plugin_abi/plugin_abi.h"
#include <absl/status/status.h>
#include <absl/status/statusor.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <string_view>
#include <variant>
#include <vector>
namespace msk::ir {

// --- PORT ---

auto Port::EstablishConnection(Port &other) -> absl::Status {
  spdlog::debug("Establishing connection between two ports: {} and {}",
                reinterpret_cast<void *>(this),
                reinterpret_cast<void *>(&other));
  if (std::holds_alternative<std::vector<ConnectionPointer>>(
          other.connection)) {
    return absl::InvalidArgumentError(std::format(
        "Tried adding incoming connection to right port (left: '{}' right: "
        "'{}')!",
        reinterpret_cast<void *>(&other), reinterpret_cast<void *>(this)));
  }

  if (!std::holds_alternative<std::vector<ConnectionPointer>>(
          this->connection)) {
    return absl::InvalidArgumentError(std::format(
        "Tried adding outgoing connection to left port (left: '{}' right: "
        "'{}')!",
        reinterpret_cast<void *>(&other), reinterpret_cast<void *>(this)));
  }

  auto &right_conn = std::get<std::vector<ConnectionPointer>>(this->connection);
  auto &left_conn = std::get<ConnectionPointer>(other.connection);

  // if (this->connection) {
  //   auto status = absl::AlreadyExistsError("Port already has a connection!");
  //   status.SetPayload("mollusk.ir/AlreadyExistsReason", absl::Cord("this"));
  //
  //   return status;
  // }
  if (left_conn) {
    auto status = absl::AlreadyExistsError("Port already has a connection!");
    status.SetPayload("mollusk.ir/AlreadyExistsReason", absl::Cord("other"));

    return status;
  }

  // NOTE: Heterogenous connections are now allowed but require type resolution
  // in evaluation if (this->dataType != other.dataType) {
  //   return absl::InvalidArgumentError(
  //       "Cannot establish connection between ports of differing datatypes!");
  // }

  auto c = std::make_shared<Connection>(&other, this);

  right_conn.push_back(c);
  other.connection = c;

  return absl::OkStatus();
}

auto Port::GetName() -> absl::StatusOr<std::string> {
  std::string name = "val_";

  if (this->IsLeft()) {
    if (auto &c = std::get<ConnectionPointer>(this->connection)) {
      if (auto *&r = c.get()->right_port) {
        name += std::format("{}", reinterpret_cast<void *>(r));
        return name;
      }
    } else {
      return absl::InvalidArgumentError(std::format(
          "Cannot get Name of a left port '{}' without a connection!",
          reinterpret_cast<void *>(this)));
    }
  }
  name += std::format("{}", reinterpret_cast<void *>(this));
  return name;
}

// --- NODE ---

auto addPort(Node::MapType &map, bool left, std::string_view name,
             std::string_view data_type) -> absl::Status {
  if (map.contains(name)) {
    return absl::AlreadyExistsError("Port already exists!");
  }

  map[std::string(name)] = std::make_unique<Port>(std::string(data_type), left);
  return absl::OkStatus();
}

auto Node::AddLeftPort(std::string_view name, std::string_view data_type)
    -> absl::StatusOr<Port *> {
  auto status = addPort(this->leftPorts, true, name, data_type);

  if (!status.ok()) {
    return status;
  }

  auto res = this->GetLeftPort(name);
  if (!res.ok())
    return res;
  spdlog::debug("Added Left port '{}' ({}) of type '{}' to node '{}'", name,
                reinterpret_cast<void *>(*res), data_type,
                reinterpret_cast<void *>(this));

  return res;
}
auto Node::AddRightPort(std::string_view name, std::string_view data_type)
    -> absl::StatusOr<Port *> {
  auto status = addPort(this->rightPorts, false, name, data_type);

  if (!status.ok()) {
    return status;
  }

  auto res = this->GetRightPort(name);
  if (!res.ok())
    return res;
  spdlog::debug("Added Right port '{}' ({}) of type '{}' to node '{}'", name,
                reinterpret_cast<void *>(*res), data_type,
                reinterpret_cast<void *>(this));

  return res;
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

template <class T, class... Args>
auto Graph::addNode(std::string_view name, Args... args)
    -> absl::StatusOr<T *> {
  if (this->subnodes.contains(name)) {
    return absl::AlreadyExistsError("Node already exists!");
  }

  auto entry = this->subnodes.try_emplace(std::string(name), T(args...));

  auto node_status = this->GetNode<T>(name);

  if (!entry.second) {
    return absl::InternalError("An error occured while adding Node!");
  }

  // spdlog::debug("Added node '{}' {}", name,
  //               reinterpret_cast<void *>(&entry.first));

  return node_status;
  // return std::get<T *>(*entry.second);
}

auto Graph::AddModule(std::string_view name, ModuleFunc impl)
    -> absl::StatusOr<Module *> {
  auto res = this->addNode<Module>(name, impl);
  if (!res.ok())
    return res;
  spdlog::debug("Added module '{}' ({}) to graph '{}'", name,
                reinterpret_cast<void *>(*res), reinterpret_cast<void *>(this));
  return res;
}

auto Graph::AddGraph(std::string_view name) -> absl::StatusOr<Graph *> {
  auto res = this->addNode<Graph>(name);
  if (!res.ok())
    return res;
  spdlog::debug("Added graph '{}' ({}) to graph '{}'", name,
                reinterpret_cast<void *>(*res), reinterpret_cast<void *>(this));
  return res;
}

template <class T>
auto Graph::GetNode(std::string_view name) -> absl::StatusOr<T *> {
  if (!this->subnodes.contains(name)) {
    return absl::NotFoundError("Node not found!");
  }

  if (auto *node = std::get_if<T>(&this->subnodes.at(std::string(name)))) {
    return node;
  }

  return absl::InvalidArgumentError("Node found but type doesn't match!");
}

} // namespace msk::ir
