#include "blender/ir_shim/ir_shim.hpp"
#include "blender/modules/modules.hpp"
#include "blender/root.hpp"
#include "mir/codegen.hpp"
#include "mir/node_graph/node_graph.hpp"
#include <absl/status/status_matchers.h>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <utility>
#include <variant>
#include <vector>

using namespace msk::blender;

using PortDefinition = std::pair<GraphShim::Polarity, std::string>;
using ConstantDefinition = std::pair<std::string, std::string>;
template <msk::ir::Module::GenerateTokenString impl>
auto testImpl(std::vector<PortDefinition> ports,
              std::vector<ConstantDefinition> constants) -> std::string {
  GraphShim g;

  auto mod = g.AddModule(g.GetGraph(), "test", FuncWrapper<impl>);
  ABSL_EXPECT_OK(mod);

  for (auto &pd : ports) {
    auto port = g.AddPort(*mod, pd.first, pd.second, "");
    ABSL_EXPECT_OK(port);
  }

  for (auto &cd : constants) {
    ABSL_EXPECT_OK(g.AddConstant(*mod, cd.first, cd.second));
  }

  auto raw_mod = g.GetSharedPointer()->graph->GetNode<msk::ir::Module>("test");
  ABSL_EXPECT_OK(raw_mod);

  std::vector<msk::ir::Module::Token> tokens;
  auto c_prov =
      std::make_shared<msk::ir::ContextProvider>(g.GetSharedPointer());

  auto it = std::back_inserter(tokens);
  ABSL_EXPECT_OK(raw_mod.value()->Evaluate({c_prov, it, **raw_mod}));

  auto res =
      tokens | std::views::transform([&raw_mod, &ports](auto &t) {
        if (auto *wildcard = std::get_if<msk::ir::WildcardToken>(&t)) {
          auto map = std::views::concat(raw_mod.value()->leftPorts,
                                        raw_mod.value()->rightPorts);
          if (auto entry = std::ranges::find_if(map.begin(), map.end(),
                                                [&wildcard](auto &entry) {
                                                  return entry.second.get() ==
                                                         wildcard->GetPort();
                                                });
              entry != map.end()) {
            auto &p = *entry;
            return msk::ir::TextToken(std::string(p.first));
          }
          ADD_FAILURE();
          return msk::ir::TextToken("FAILURE");
        }

        return std::get<msk::ir::TextToken>(t);
      }) |
      std::ranges::to<std::vector<msk::ir::TextToken>>();

  std::string text =
      res | std::views::transform([](auto &e) { return e.GetString(); }) |
      std::views::join | std::ranges::to<std::string>();
  return text;
}

TEST(BLENDER_MODULES, DUMMY_MODULE) {
  EXPECT_EQ(testImpl<GenerateTokenStringDummy>(
                {
                    {GraphShim::LEFT, "value0"},
                    {GraphShim::LEFT, "value1"},
                    {GraphShim::RIGHT, "value2"},
                },
                {}),
            "bla\nLeft port 'value0' resolves to: value0\nI'm not "
            "flushed!\n\nvalue2=value0+value1;\n");
}
