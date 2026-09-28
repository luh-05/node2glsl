#include "blender/root.hpp"
#include "mir/node_graph/GraphContext.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <blender/modules/modules.hpp>
#include <iterator>
#include <memory>
// #include <mir/mollusk_ir.hpp>
#include <mir/codegen.hpp>
#include <spdlog/spdlog.h>
#include <variant>
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

  auto mod = msk::ir::Module(
      msk::blender::FuncWrapper<msk::blender::GenerateTokenStringDummy>);
  auto a = context->AddConstant(&mod, "a", "4");

  auto tokens = std::vector<msk::ir::CodegenToken>();
  if (auto status =
          mod.Evaluate({context_provider, std::back_inserter(tokens), mod});
      !status.ok()) {
    spdlog::error(status.message());
  }

  std::string c;
  spdlog::warn("DummyModule generated {} tokens:", tokens.size());
  for (auto it = tokens.begin(); it != tokens.end(); it++) {
    static int i = 0;
    auto token = &*it;
    std::string token_string;
    if (auto *t = std::get_if<msk::ir::TextToken>(token)) {
      token_string = t->GetString();
    } else if (auto *t = std::get_if<msk::ir::WildcardToken>(token)) {
      if (auto s = t->GetString(); !s.ok()) {
        spdlog::error(s.status().ToString());
      } else {
        token_string = s.value();
      }
    }

    spdlog::warn("Token {}: {:?}", ++i, token_string);
    c += token_string;
  }

  spdlog::warn(std::format("Output: \n{}\n", c));

  return 0;
}
