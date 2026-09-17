#include "mir/node_graph/GraphContext.hpp"
#include <blender/modules/modules.hpp>
#include <iterator>
#include <memory>
// #include <mir/mollusk_ir.hpp>
#include <mir/codegen.hpp>
#include <spdlog/spdlog.h>
#include <vector>

int main() {
  // msk::ir::test();
  auto context = std::make_shared<msk::ir::GraphContext>();
  // auto text = std::string("a");
  // auto bkdfjg = context->const_store.GetConstant<uint32_t>(nullptr, text);

  // auto t = context->AddConstant(nullptr, "a", "123");
  // auto t2 = context->GetConstant<uint32_t>(nullptr, "a");
  // spdlog::warn(t2.value());

  auto context_provider = std::make_shared<msk::ir::ContextProvider>(context);

  auto mod = msk::ir::Module(msk::blender::GenerateTokenStringDummy);
  spdlog::warn(mod.id);
  auto a = context->AddConstant(&mod, "a", "4");

  auto token_string = std::vector<msk::ir::Module::Token>();
  if (auto status =
          mod.impl({context_provider, std::back_inserter(token_string), mod});
      !status.ok()) {
    spdlog::error(status.message());
  }

  std::string c;
  spdlog::warn("DummyModule generated {} tokens:", token_string.size());
  for (auto it = token_string.begin(); it != token_string.end(); it++) {
    static int i = 0;
    spdlog::warn("Token {}: {:?}", ++i, it->get()->GetString());
    c += it->get()->GetString();
  }

  spdlog::warn(std::format("Output: \n{}\n", c));

  return 0;
}
