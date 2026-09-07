#include "mir/codegen.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <blender/modules/modules.hpp>
#include <ir/graph/graph.hpp>
#include <iterator>
#include <memory>
#include <mir/mollusk_ir.hpp>
#include <spdlog/spdlog.h>
#include <vector>

int main() {
  msk::ir::test();
  auto dummy = msk::blender::DummyModule();
  spdlog::warn(dummy.id);
  dummy = msk::blender::DummyModule();
  spdlog::warn(dummy.id);

  auto context_provider = std::make_unique<msk::ir::ContextProvider>();
  auto token_string = std::vector<msk::ir::Module::Token>();
  if (auto status = dummy.GenerateTokenString(
          context_provider.get(), {std::back_inserter(token_string), dummy});
      !status.ok()) {
    spdlog::error(status.message());
  }

  spdlog::warn("DummyModule generated {} tokens:", token_string.size());
  for (auto it = token_string.begin(); it != token_string.end(); it++) {
    static int i = 0;
    spdlog::warn("Token {}: {:?}", ++i, it->get()->GetString());
  }

  return 0;
}
