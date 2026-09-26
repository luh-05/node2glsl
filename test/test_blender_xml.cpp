#include "blender/ir_shim/ir_shim.hpp"
#include "blender/modules/modules.hpp"
#include "blender/root.hpp"
#include "blender/xml/xml.hpp"
#include "mir/node_graph/node_graph.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include <absl/status/status.h>
#include <absl/status/status_matchers.h>
#include <absl/status/statusor.h>
#include <format>
#include <gtest/gtest.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

TEST(BLENDER_GRAPH_TEST, GRAPH_SHIM) {
  using GraphShim = msk::blender::GraphShim;
  using ModuleHandle = msk::blender::ModuleHandle;
  using PortHandle = msk::blender::PortHandle;

  GraphShim g;

  auto foo0 = g.AddModule(
      g.GetGraph(), "foo.0",
      msk::blender::FuncWrapper<msk::blender::GenerateTokenStringDummy>);
  ABSL_EXPECT_OK(foo0);
  auto foo1 = g.AddModule(
      g.GetGraph(), "foo.1",
      msk::blender::FuncWrapper<msk::blender::GenerateTokenStringDummy>);
  ABSL_EXPECT_OK(foo1);

  EXPECT_FALSE(
      g.AddModule(
           g.GetGraph(), "foo.0",
           msk::blender::FuncWrapper<msk::blender::GenerateTokenStringDummy>)
          .ok());

  auto foo0_again = g.GetModule(g.GetGraph(), "foo.0");
  ABSL_EXPECT_OK(foo0_again);

  auto foo0_value0 = g.AddPort(*foo0_again, GraphShim::RIGHT, "value0", "");
  ABSL_EXPECT_OK(foo0_value0);

  auto bar0 = g.AddSubGraph(g.GetGraph(), "bar");
  ABSL_EXPECT_OK(bar0);

  auto bar0_value0 = g.AddPort(*bar0, GraphShim::LEFT, "value0", "");
  ABSL_EXPECT_OK(bar0_value0);

  ABSL_EXPECT_OK(g.ConnectPorts(*foo0_value0, *bar0_value0));
}

TEST(BLENDER_GRAPH_TEST, READ_GRAPH) {
  msk::PluginStore store;
  ABSL_EXPECT_OK(store.FetchPlugin());

  msk::blender::XMLParser parser(&store);

  std::string xml_text_file =
      std::string(TEST_DATA_DIR) + "/test_blender_xml.xml";
  ABSL_EXPECT_OK(parser.XMLread(xml_text_file));
}

TEST(BLENDER_GRAPH_TEST, PARSE_GRAPH) {

  msk::PluginStore store;
  ABSL_EXPECT_OK(store.FetchPlugin());

  msk::blender::XMLParser parser(&store);

  std::string xml_text_file =
      std::string(TEST_DATA_DIR) + "/test_blender_xml.xml";
  ABSL_EXPECT_OK(parser.XMLread(xml_text_file));

  msk::ContextPointer context;
  {
    auto s = parser.ParseGraph("0");
    ABSL_EXPECT_OK(s);
    context = s.value();
  }
}

TEST(BLENDER_GRAPH_TEST, EVALUATE_GRAPH_LINEAR) {

  msk::PluginStore store;
  ABSL_EXPECT_OK(store.FetchPlugin());

  msk::blender::XMLParser parser(&store);

  std::string xml_text_file =
      std::string(TEST_DATA_DIR) + "/test_blender_xml.xml";
  ABSL_EXPECT_OK(parser.XMLread(xml_text_file));

  msk::ContextPointer context;
  {
    auto s = parser.ParseGraph("0");
    ABSL_EXPECT_OK(s);
    context = s.value();
  }

  msk::Evaluator eval(context,
                      std::make_unique<msk::ForwardEvaluationStrategy>());

  std::string res;
  ABSL_EXPECT_OK(eval.Evaluate(res));
}
