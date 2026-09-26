
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

  *inserter =
      std::format("// Module '{}'\n{{\n", reinterpret_cast<void *>(&module));

  if (auto s = module.Evaluate({this->cxt_prov, inserter, module}); !s.ok()) {
    return absl::AbortedError(
        std::format("Failed to evaluate Module: {}", s.ToString()));
  }

  auto last_token = &*v.rbegin();

  // if (this->pretty) {
  //   if (std::holds_alternative<msk::ir::WildcardToken>(*last_token)) {
  //     *inserter = ir::TextToken("\n");
  //   } else if (auto *tt = std::get_if<msk::ir::TextToken>(last_token)) {
  //     if (*tt->GetString().rbegin() != '\n') {
  //       *inserter = ir::TextToken("\n");
  //     }
  //   } else {
  //     return absl::InternalError("Failed to match token type!");
  //   }
  // }

  *inserter = ir::TextToken("}\n");

  // Convert port access to Connection access logs
  // Also discard any module that does not affect any connections
  if (auto mod_info = cxt->GetModuleInfo(&module).value_or(nullptr)) {
    auto conn_log = std::make_shared<ConnectionAccessLog>();

    // Add all left connections
    for (auto left_port : mod_info->first) {
      if (auto *conn = std::get_if<msk::ir::Port::ConnectionPointer>(
              &left_port->connection)) {
        conn_log->first.insert(conn->get());
      }
    }

    for (auto right_port : mod_info->second) {
      // Add right ports to list to generate constants
      this->right_ports.insert(right_port);

      // Add all right connections
      if (auto *conn =
              std::get_if<std::vector<msk::ir::Port::ConnectionPointer>>(
                  &right_port->connection)) {
        std::for_each(conn->begin(), conn->end(), [&conn_log](auto &e) {
          if (!e)
            return;
          conn_log->second.insert(e.get());
        });
      }
    }

    this->vectors.push_back(std::move(v));
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
    for (auto right_conenction : ordering.first->second) {
      auto entry = ownership.find(right_conenction);
      if (entry != ownership.end()) {
        return absl::InternalError("Two indices claim to own the same port!");
      }
      auto r = ownership.try_emplace(right_conenction, ordering.second);
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
    for (auto left_connection : ordering.first->first) {
      auto owner = ownership.find(left_connection);
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

      // Otherwise move down dependency chain
      idx = *deps->second.begin();
    }
    if (visited_count >= indices_left.size()) {
      return absl::InternalError("Found dependency loop!");
    }
    indices_left.erase(idx);
    order.push_back(idx);
  }

  // Reorder schedule accordingly
  auto old_schedule = std::move(this->schedule);
  this->schedule = order |
                   std::views::transform([&old_schedule](const auto &i) {
                     return std::move(old_schedule[i]);
                   }) |
                   std::ranges::to<Schedule>();

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::genGlobal(
    msk::ir::Port *port, std::back_insert_iterator<TokenVector> &it)
    -> absl::Status {
  if (auto s = port->GetName(); s.ok()) {
    *it = port->GetDataType() + " " + s.value() + ";\n";
    return absl::OkStatus();
  } else
    return absl::AbortedError(std::format(
        "While generating global definition: {}", s.status().ToString()));
}

auto ForwardEvaluationStrategy::genGlobals(
    std::back_insert_iterator<TokenVector> &it) -> absl::Status {
  for (auto *p : this->right_ports) {
    if (auto s = this->genGlobal(p, it); !s.ok()) {
      return s;
    }
  }

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::EvaluateTokens(ContextPointer cxt,
                                               std::string &out)
    -> absl::Status {
  // Order Schedule first
  if (auto s = this->orderSchedule(); !s.ok()) {
    return absl::InvalidArgumentError(
        std::format("Graph could not be serialized: {}", s.ToString()));
  }

  auto vit = std::back_inserter(this->tokens);
  *vit = std::string(
      "// --- GENERATED CODE, DO NOT EDIT ---\n// Global definitions\n\n");

  // Then create global definitions
  if (auto s = this->genGlobals(vit); !s.ok()) {
    return s;
  }

  *vit = std::string("\n");

  // Add function header
  *vit = std::string("// Main Method\n");
  *vit = std::string("int main() {\n");

  // Then append ordered modules
  *vit = std::string("// Modules\n\n");

  auto schedule_second =
      this->schedule | std::views::transform([](auto &e) { return e.second; });

  for (auto idx : schedule_second) {
    this->tokens.append_range(vectors.at(idx));
  }

  // Finish with main function footer
  *vit = std::string("}\n");

  // Then generate glsl from final token vector
  for (auto it = tokens.begin(); it != tokens.end(); it++) {
    auto token = &*it;
    std::string glsl_string;
    if (auto *t = std::get_if<msk::ir::TextToken>(token)) {
      glsl_string = t->GetString();
    } else if (auto *t = std::get_if<msk::ir::WildcardToken>(token)) {
      if (auto s = t->GetString(); !s.ok()) {
        return absl::AbortedError(
            std::format("While evaluating tokens: {}", s.status().ToString()));
      } else {
        glsl_string = s.value();
      }
    } else {
      return absl::InvalidArgumentError(
          std::format("Invalid Token Type in Token Evaluation: '{}'",
                      typeid(token).name()));
    }
    out += glsl_string;
  }

  return absl::OkStatus();
}

auto ForwardEvaluationStrategy::prettify(std::string &glsl) -> absl::Status {
  return absl::UnimplementedError(
      "ForwardEvaluationStrategy::prettify is not implemented yet");

  std::string p;

  size_t scope = 0;
  for (auto line : glsl | std::views::split('\n')) {
    for (auto br_open : line | std::views::split('{')) {
      p.append_range(br_open);
      p.append("{\n");
      scope++;
    }
  }

  return absl::OkStatus();
}

} // namespace msk
