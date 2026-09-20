
#include "mir/codegen.hpp"
#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include <absl/status/status.h>
#include <algorithm>
#include <bitset>
#include <format>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <ranges>
#include <spdlog/spdlog.h>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
namespace msk {

auto ForwardEvaluationStrategy::evalGraph(ContextPointer cxt, ir::Graph &graph)
    -> absl::Status {
  // for (auto it = cxt->graph->GetSubnodesIt();
  //      it != cxt->graph->GetSubnodesItEnd(); it++) {
  //   auto value = &it->second;
  //
  //   if (auto &g = std::get_if<ir::Graph>(value)) {
  //     if (g) {
  //       if (auto s = evalGraph(cxt, *g); !s.ok()) {
  //         return s;
  //       }
  //     } else {
  //       return absl::NotFoundError(
  //           std::format("The graph '{}', is nullptr!", it->first));
  //     }
  //   } else if (auto &m = std::get_if<ir::Module>(value)) {
  //     if (m) {
  //       if (auto s = evalModule(cxt, m); !s.ok()) {
  //         return s;
  //       }
  //     } else {
  //       return absl::NotFoundError(
  //           std::format("The node '{}', is nullptr!", it->first));
  //     }
  //   } else {
  //     return absl::InternalError(
  //         "Could not determine variant type to be neither Graph nor
  //         Module!");
  //   }
  // }
  auto &subnodes = *graph.GetSubnodes();
  for (auto &[key, value] : subnodes) {

    if (auto *g = std::get_if<ir::Graph>(&value)) {
      if (g) {
        if (auto s = evalGraph(cxt, *g); !s.ok()) {
          return s;
        }
      } else {
        return absl::NotFoundError(
            std::format("The graph '{}', is nullptr!", key));
      }
    } else if (auto *m = std::get_if<ir::Module>(&value)) {
      if (m) {
        if (auto s = evalModule(cxt, *m); !s.ok()) {
          return s;
        }
      } else {
        return absl::NotFoundError(
            std::format("The node '{}', is nullptr!", key));
      }
    } else {
      return absl::InternalError(
          "Could not determine variant type to be neither Graph nor Module!");
    }
  }

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::evalModule(ContextPointer cxt,
                                           ir::Module &module) -> absl::Status {
  TokenVector v;
  auto inserter = std::back_inserter(v);

  *inserter = ir::TextToken("{\n");

  if (auto s = module.Evaluate({this->cxt_prov, inserter, module}); !s.ok()) {
    return absl::AbortedError(
        std::format("Failed to evaluate Module: {}", s.ToString()));
  }

  *inserter = ir::TextToken("}\n");

  if (auto mod_info = cxt->GetModuleInfo(&module); mod_info.has_value()) {
    this->vectors.push_back(std::move(v));

    auto conn_log = std::make_shared<ConnectionAccessLog>();
    conn_log->first = ConnectionSet();
    conn_log->second = ConnectionSet();
    for (auto left_port : mod_info.value()->first) {
      conn_log->first.insert(
          std::get<msk::ir::Port::ConnectionPointer>(left_port->connection)
              .get());
    }
    for (auto right_port : mod_info.value()->second) {
      if (!right_port)
        continue;
      if (auto *conn =
              std::get_if<std::vector<msk::ir::Port::ConnectionPointer>>(
                  &right_port->connection)) {
        // if (!conn || conn->empty())
        //   break;
        std::for_each(conn->begin(), conn->end(), [&conn_log](auto &e) {
          conn_log->second.insert(e.get());
        });
        // if (conn_log->second.empty()) {
        // return absl::InternalError("sdfsdaf");
        // }
        // for (auto e = conn->begin(); e != conn->end(); e++) {
        //   // conn_log.second.insert(e->get());
        // }
      }
    }

    this->schedule.push_back({std::move(conn_log), this->vectors.size() - 1});
  }

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::GenerateTokens(ContextPointer cxt)
    -> absl::Status {
  this->cxt_prov = std::make_shared<ir::ContextProvider>(cxt);
  if (auto s = this->evalGraph(cxt, *cxt->graph.get()); !s.ok()) {
    return s;
  }
  return absl::OkStatus();
}

// Orders schedule
auto ForwardEvaluationStrategy::orderSchedule() -> absl::Status {
  using Index = size_t;
  using ConnectionOwnership = std::map<msk::ir::Connection *, Index>;
  // Map denoting dependencies
  ConnectionOwnership ownership;

  // Fill in deps map using connection access logs
  for (auto ordering : this->schedule) {
    for (auto right_port : ordering.first->second) {
      auto entry = ownership.find(right_port);
      if (entry != ownership.end()) {
        return absl::InternalError("Two indices claim to own the same port!");
      }
      auto r = ownership.try_emplace(right_port, ordering.second);
      if (!r.second) {
        return absl::InternalError("Failed to add IndexSet while ordering!");
      }
    }
  }

  // using Dependency = std::pair<Index, Index>;
  using DependencyVector = std::unordered_set<Index>;
  using DependencyMap = std::map<Index, DependencyVector>;
  // Map holding all dependencies
  DependencyMap dependencies;
  for (auto ordering : this->schedule) {
    for (auto left_port : ordering.first->first) {
      auto owner = ownership.find(left_port);
      if (owner == ownership.end())
        continue;

      auto dep = dependencies.find(ordering.second);
      if (dep == dependencies.end()) {
        auto res =
            dependencies.try_emplace(ordering.second, DependencyVector());
        if (!res.second) {
          return absl::InternalError(
              "Failed to add DependencyVector to dependencies!");
        }
        dep = res.first;
      }

      dep->second.insert(owner->second);
    }
  }

  using IndexSet = std::unordered_set<Index>;
  // Calculate valid order based on deps
  IndexSet indices_left(std::from_range,
                        std::views::iota(size_t{0}, this->schedule.size()));
  using IndexVector = std::vector<Index>;
  IndexVector order;

  while (!indices_left.empty()) {
    // Get top element
    auto idx = *indices_left.begin();
    // indices_left.erase(indices_left.begin());

    // Look for conflicts
    size_t visited_count = 0;
    for (; visited_count < indices_left.size(); visited_count++) {
      auto deps = dependencies.find(idx);
      // When no incoming dependencies are registered, accept
      if (deps == dependencies.end() || deps->second.empty()) {
        // Remove outgoing dependencies
        std::for_each(dependencies.begin(), dependencies.end(),
                      [&idx](auto &out_dep) { out_dep.second.erase(idx); });

        // break from loop
        break;
      }

      // Otherwise move to next layer
      idx = *deps->second.begin();
    }
    if (visited_count >= indices_left.size()) {
      return absl::InternalError("Found dependency loop!");
    }
    indices_left.erase(idx);
    order.push_back(idx);
  }

  // Reorder schedule accordingly
  Schedule old_schedule = std::move(this->schedule);
  auto mapped_indices =
      order | std::views::transform([&old_schedule](auto &p) -> Index & {
        return old_schedule[p].second;
      });
  this->schedule = Schedule();
  std::for_each(mapped_indices.begin(), mapped_indices.end(),
                [&old_schedule, this](auto &idx) {
                  this->schedule.push_back(std::move(old_schedule[idx]));
                });

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::EvaluateTokens(ContextPointer cxt,
                                               std::string &out)
    -> absl::Status {
  if (auto s = this->orderSchedule(); !s.ok()) {
    return absl::InvalidArgumentError(
        std::format("Graph could not be serialized: {}", s.ToString()));
  }

  auto schedule_second =
      this->schedule | std::views::transform([](auto &e) { return e.second; });

  for (auto idx : schedule_second) {
    this->tokens.append_range(vectors.at(idx));
  }

  for (auto it = tokens.begin(); it != tokens.end(); it++) {
    // static int i = 0;
    auto token = &*it;
    std::string token_string;
    if (auto *t = std::get_if<msk::ir::TextToken>(token)) {
      token_string = t->GetString();
    } else if (auto *t = std::get_if<msk::ir::WildcardToken>(token)) {
      token_string = t->GetString();
    } else {
      return absl::InvalidArgumentError(
          std::format("Invalid Token Type in Token Evaluation: '{}'",
                      typeid(token).name()));
    }
    out += token_string;
  }

  return absl::OkStatus();
}

} // namespace msk
