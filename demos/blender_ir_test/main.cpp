#include <blender/modules/modules.hpp>
#include <ir/graph/graph.hpp>
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
  auto token_string = dummy.GenerateTokenString(context_provider.get());
  if (auto status = token_string.status(); !status.ok()) {
    spdlog::error(status.message());
  }

  spdlog::warn("DummyModule generated {} tokens:", token_string.value().size());
  for (auto it = token_string.value().begin(); it != token_string.value().end();
       it++) {
    static int i = 0;
    spdlog::warn("Token {}: '{}'", ++i, it->get()->GetString());
  }

  return 0;
}
