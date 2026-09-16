#include <blender/modules/modules.hpp>
#include <ir/graph/graph.hpp>
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

  auto dummy = msk::blender::DummyModule("foo");
  spdlog::warn(dummy.id);
  auto a = context->AddConstant(&dummy, "a", "4");

  auto token_string = std::vector<msk::ir::Module::Token>();
  if (auto status = dummy.GenerateTokenString(
          {context_provider, std::back_inserter(token_string), dummy});
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
