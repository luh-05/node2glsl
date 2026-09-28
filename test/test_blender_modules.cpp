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
#include <system_error>
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

//=========================================================================
// DummyModule
//=========================================================================

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

//=========================================================================
// FunctionNodeBitMath
//=========================================================================

TEST(BLENDER_MODULES, FUNCTION_NODE_BIT_MATH_NOT) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeBitMath>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::RIGHT, "Value0"},
                },
                {{"operation0", "NOT"}}),
            "Value0= ~A0;");
}

auto functionNodeBitMathHelper(std::string operation) {
  std::string op_c;

  if (operation == "AND") {
    op_c = "&";
  } else if (operation == "OR") {
    op_c = "|";
  } else if (operation == "XOR") {
    op_c = "^";
  }

  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeBitMath>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::RIGHT, "Value0"},
                },
                {{"operation0", operation}}),
            "Value0=A0" + op_c + "B0;");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BIT_MATH_AND) {
  functionNodeBitMathHelper("AND");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BIT_MATH_OR) {
  functionNodeBitMathHelper("OR");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BIT_MATH_XOR) {
  functionNodeBitMathHelper("XOR");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BIT_MATH_SHIFT) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeBitMath>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::LEFT, "Shift0"},
                    {GraphShim::RIGHT, "Value0"},
                },
                {{"operation0", "SHIFT"}}),
            "if (Shift0 > 0) {Value0=A0<<Shift0;}else if (Shift0 < 0) "
            "{Value0=A0>>(-Shift0);}else {Value0=A0;}");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BIT_MATH_ROTATE) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeBitMath>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::LEFT, "Shift0"},
                    {GraphShim::RIGHT, "Value0"},
                },
                {{"operation0", "ROTATE"}}),
            "if (Shift0 > 0) {Value0=(A0<<Shift0) | (A0>> (32 - Shift0));}else "
            "if (Shift0 < 0) {Value0=(A0>> (-Shift0)) | (A0<< (32 + "
            "Shift0));}else {Value0=A0;}");
}

//=========================================================================
// FunctionNodeFloatToInt
//=========================================================================

auto functionNodeFloatToIntHelper(std::string operation) {
  std::string function;

  if (operation == "ROUND") {
    function = "round";
  } else if (operation == "FLOOR") {
    function = "floor";
  } else if (operation == "CEILING") {
    function = "ceil";
  } else if (operation == "TRUNCATE") {
    function = "trunc";
  }

  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeFloatToInt>(
                {
                    {GraphShim::LEFT, "Float0"},
                    {GraphShim::RIGHT, "Integer0"},
                },
                {{"rounding_mode0", operation}}),
            "Integer0 = int(" + function + "(Float0));");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_FLOAT_TO_INT_ROUND) {
  functionNodeFloatToIntHelper("ROUND");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_FLOAT_TO_INT_FLOOR) {
  functionNodeFloatToIntHelper("FLOOR");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_FLOAT_TO_INT_CEILING) {
  functionNodeFloatToIntHelper("CEILING");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_FLOAT_TO_INT_TRUNCATE) {
  functionNodeFloatToIntHelper("TRUNCATE");
}

//=========================================================================
// FunctionNodeCompare
//=========================================================================

auto functionNodeCompareHelper(std::string operation, std::string data_type) {
  std::string op_c;

  if (operation == "LESS_THAN") {
    op_c = "<";
  } else if (operation == "LESS_EQUAL") {
    op_c = "<=";
  } else if (operation == "GREATER_THAN") {
    op_c = ">";
  } else if (operation == "GREATER_EQUAL") {
    op_c = ">=";
  } else if (operation == "EQUAL") {
    op_c = "==";
  } else if (operation == "NOT_EQUAL") {
    op_c = "!=";
  }

  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::RIGHT, "Result0"},
                },
                {{"operation0", operation}, {"data_type0", data_type}}),
            "Result0=A0" + op_c + "B0;");
}

//============================= Integer ===================================

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_INT_LESS_THAN) {
  functionNodeCompareHelper("LESS_THAN", "INT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_INT_LESS_EQUAL) {
  functionNodeCompareHelper("LESS_EQUAL", "INT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_INT_GREATER_THAN) {
  functionNodeCompareHelper("GREATER_THAN", "INT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_INT_GREATER_EQUAL) {
  functionNodeCompareHelper("GREATER_EQUAL", "INT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_INT_EQUAL) {
  functionNodeCompareHelper("EQUAL", "INT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_INT_NOT_EQUAL) {
  functionNodeCompareHelper("NOT_EQUAL", "INT");
}

//============================= Float =====================================

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_FLOAT_LESS_THAN) {
  functionNodeCompareHelper("LESS_THAN", "FLOAT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_FLOAT_LESS_EQUAL) {
  functionNodeCompareHelper("LESS_EQUAL", "FLOAT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_FLOAT_GREATER_THAN) {
  functionNodeCompareHelper("GREATER_THAN", "FLOAT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_FLOAT_GREATER_EQUAL) {
  functionNodeCompareHelper("GREATER_EQUAL", "FLOAT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_FLOAT_EQUAL) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::LEFT, "Epsilon0"},
                    {GraphShim::RIGHT, "Result0"},
                },
                {
                    {"operation0", "EQUAL"},
                    {"data_type0", "FLOAT"},
                }),
            "Result0= abs(A0-B0)<=Epsilon0;");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_FLOAT_NOT_EQUAL) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::LEFT, "Epsilon0"},
                    {GraphShim::RIGHT, "Result0"},
                },
                {
                    {"operation0", "NOT_EQUAL"},
                    {"data_type0", "FLOAT"},
                }),
            "Result0= abs(A0-B0)>Epsilon0;");
}

//============================== Vector ===================================

//============================ Dot Product ================================

auto functionNodeCompareVectorDotProductHelper(std::string operation) {
  std::string op_c;

  if (operation == "LESS_THAN") {
    op_c = "<";
  } else if (operation == "LESS_EQUAL") {
    op_c = "<=";
  } else if (operation == "GREATER_THAN") {
    op_c = ">";
  } else if (operation == "GREATER_EQUAL") {
    op_c = ">=";
  } else if (operation == "EQUAL") {
    op_c = "<=";
  } else if (operation == "NOT_EQUAL") {
    op_c = ">";
  }

  if (operation == "EQUAL" || operation == "NOT_EQUAL") {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::LEFT, "Epsilon0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "DOT_PRODUCT"}}),
              "Result0= abs(dot(A0, B0) - C0) " + op_c + " Epsilon0;");
  } else {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "DOT_PRODUCT"}}),
              "Result0= dot(A0, B0) " + op_c + " C0;");
  }
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DOT_PRODUCT_LESS_THAN) {
  functionNodeCompareVectorDotProductHelper("LESS_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DOT_PRODUCT_LESS_EQUAL) {
  functionNodeCompareVectorDotProductHelper("LESS_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DOT_PRODUCT_GREATER_THAN) {
  functionNodeCompareVectorDotProductHelper("GREATER_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DOT_PRODUCT_GREATER_EQUAL) {
  functionNodeCompareVectorDotProductHelper("GREATER_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DOT_PRODUCT_EQUAL) {
  functionNodeCompareVectorDotProductHelper("EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DOT_PRODUCT_NOT_EQUAL) {
  functionNodeCompareVectorDotProductHelper("NOT_EQUAL");
}

//============================= Direction =================================

auto functionNodeCompareVectorDirectionHelper(std::string operation) {
  std::string op_c;

  if (operation == "LESS_THAN") {
    op_c = "<";
  } else if (operation == "LESS_EQUAL") {
    op_c = "<=";
  } else if (operation == "GREATER_THAN") {
    op_c = ">";
  } else if (operation == "GREATER_EQUAL") {
    op_c = ">=";
  } else if (operation == "EQUAL") {
    op_c = "<=";
  } else if (operation == "NOT_EQUAL") {
    op_c = ">";
  }

  if (operation == "EQUAL" || operation == "NOT_EQUAL") {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::LEFT, "Angle0"},
                      {GraphShim::LEFT, "Epsilon0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "DIRECTION"}}),
              "Result0= abs(acos(clamp(dot(normalize(A0), normalize(B0)), "
              "-1.0, 1.0)) - Angle0) " +
                  op_c + " Epsilon0;");
  } else {
    EXPECT_EQ(
        testImpl<GenerateTokenStringFunctionNodeCompare>(
            {
                {GraphShim::LEFT, "A0"},
                {GraphShim::LEFT, "B0"},
                {GraphShim::LEFT, "C0"},
                {GraphShim::LEFT, "Angle0"},
                {GraphShim::RIGHT, "Result0"},
            },
            {{"operation0", operation},
             {"data_type0", "VECTOR"},
             {"mode0", "DIRECTION"}}),
        "Result0= acos(clamp(dot(normalize(A0), normalize(B0)), -1.0, 1.0)) " +
            op_c + " Angle0;");
  }
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DIRECTION_LESS_THAN) {
  functionNodeCompareVectorDirectionHelper("LESS_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DIRECTION_LESS_EQUAL) {
  functionNodeCompareVectorDirectionHelper("LESS_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DIRECTION_GREATER_THAN) {
  functionNodeCompareVectorDirectionHelper("GREATER_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DIRECTION_GREATER_EQUAL) {
  functionNodeCompareVectorDirectionHelper("GREATER_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DIRECTION_EQUAL) {
  functionNodeCompareVectorDirectionHelper("EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_DIRECTION_NOT_EQUAL) {
  functionNodeCompareVectorDirectionHelper("NOT_EQUAL");
}

//============================== Element ==================================

auto functionNodeCompareVectorElementHelper(std::string operation) {
  std::string op_c;
  std::string logical_op;

  if (operation == "LESS_THAN") {
    op_c = "<";
  } else if (operation == "LESS_EQUAL") {
    op_c = "<=";
  } else if (operation == "GREATER_THAN") {
    op_c = ">";
  } else if (operation == "GREATER_EQUAL") {
    op_c = ">=";
  } else if (operation == "EQUAL") {
    op_c = "<=";
    logical_op = " && ";
  } else if (operation == "NOT_EQUAL") {
    op_c = ">";
    logical_op = " || ";
  }

  if (operation == "EQUAL" || operation == "NOT_EQUAL") {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::LEFT, "Epsilon0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "ELEMENT"}}),
              "Result0= (abs(A0.x - B0.x) " + op_c + " Epsilon0)" + logical_op +
                  "(abs(A0.y - B0.y) " + op_c + " Epsilon0)" + logical_op +
                  "(abs(A0.z - B0.z) " + op_c + " Epsilon0);");
  } else {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "ELEMENT"}}),
              "Result0= (A0.x " + op_c + " B0.x) && (A0.y " + op_c +
                  " B0.y) && (A0.z " + op_c + " B0.z);");
  }
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_ELEMENT_LESS_THAN) {
  functionNodeCompareVectorElementHelper("LESS_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_ELEMENT_LESS_EQUAL) {
  functionNodeCompareVectorElementHelper("LESS_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_ELEMENT_GREATER_THAN) {
  functionNodeCompareVectorElementHelper("GREATER_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_ELEMENT_GREATER_EQUAL) {
  functionNodeCompareVectorElementHelper("GREATER_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_ELEMENT_EQUAL) {
  functionNodeCompareVectorElementHelper("EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_ELEMENT_NOT_EQUAL) {
  functionNodeCompareVectorElementHelper("NOT_EQUAL");
}

//============================== Length ===================================

auto functionNodeCompareVectorLengthHelper(std::string operation) {
  std::string op_c;

  if (operation == "LESS_THAN") {
    op_c = "<";
  } else if (operation == "LESS_EQUAL") {
    op_c = "<=";
  } else if (operation == "GREATER_THAN") {
    op_c = ">";
  } else if (operation == "GREATER_EQUAL") {
    op_c = ">=";
  } else if (operation == "EQUAL") {
    op_c = "<=";
  } else if (operation == "NOT_EQUAL") {
    op_c = ">";
  }

  if (operation == "EQUAL" || operation == "NOT_EQUAL") {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::LEFT, "Epsilon0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "LENGTH"}}),
              "Result0= abs(length(A0) - length(B0)) " + op_c + " Epsilon0;");
  } else {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "LENGTH"}}),
              "Result0= length(A0) " + op_c + " length(B0);");
  }
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_LENGTH_LESS_THAN) {
  functionNodeCompareVectorLengthHelper("LESS_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_LENGTH_LESS_EQUAL) {
  functionNodeCompareVectorLengthHelper("LESS_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_LENGTH_GREATER_THAN) {
  functionNodeCompareVectorLengthHelper("GREATER_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_LENGTH_GREATER_EQUAL) {
  functionNodeCompareVectorLengthHelper("GREATER_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_LENGTH_EQUAL) {
  functionNodeCompareVectorLengthHelper("EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_LENGTH_NOT_EQUAL) {
  functionNodeCompareVectorLengthHelper("NOT_EQUAL");
}

//============================== Average ==================================

auto functionNodeCompareVectorAverageHelper(std::string operation) {
  std::string op_c;

  if (operation == "LESS_THAN") {
    op_c = "<";
  } else if (operation == "LESS_EQUAL") {
    op_c = "<=";
  } else if (operation == "GREATER_THAN") {
    op_c = ">";
  } else if (operation == "GREATER_EQUAL") {
    op_c = ">=";
  } else if (operation == "EQUAL") {
    op_c = "<=";
  } else if (operation == "NOT_EQUAL") {
    op_c = ">";
  }

  if (operation == "EQUAL" || operation == "NOT_EQUAL") {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::LEFT, "Epsilon0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "AVERAGE"}}),
              "Result0= abs(((A0.x + A0.y + A0.z) / 3.0) - ((B0.x + B0.y + "
              "B0.z) / 3.0)) " +
                  op_c + " Epsilon0;");
  } else {
    EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeCompare>(
                  {
                      {GraphShim::LEFT, "A0"},
                      {GraphShim::LEFT, "B0"},
                      {GraphShim::LEFT, "C0"},
                      {GraphShim::RIGHT, "Result0"},
                  },
                  {{"operation0", operation},
                   {"data_type0", "VECTOR"},
                   {"mode0", "AVERAGE"}}),
              "Result0= ((A0.x + A0.y + A0.z) / 3.0) " + op_c +
                  " ((B0.x + B0.y + B0.z) / 3.0);");
  }
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_AVERAGE_LESS_THAN) {
  functionNodeCompareVectorAverageHelper("LESS_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_AVERAGE_LESS_EQUAL) {
  functionNodeCompareVectorAverageHelper("LESS_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_AVERAGE_GREATER_THAN) {
  functionNodeCompareVectorAverageHelper("GREATER_THAN");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_AVERAGE_GREATER_EQUAL) {
  functionNodeCompareVectorAverageHelper("GREATER_EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_AVERAGE_EQUAL) {
  functionNodeCompareVectorAverageHelper("EQUAL");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_COMPARE_VECTOR_AVERAGE_NOT_EQUAL) {
  functionNodeCompareVectorAverageHelper("NOT_EQUAL");
}

//============================================================================
// FunctionNodeIntegerMath
//============================================================================

auto functionNodeIntegerMathBasicHelper(std::string operation) {
  std::string op_c;

  if (operation == "ADD") {
    op_c = "+";
  } else if (operation == "SUBTRACT") {
    op_c = "-";
  } else if (operation == "MULTIPLY") {
    op_c = "*";
  } else if (operation == "DIVIDE") {
    op_c = "/";
  } else if (operation == "MODULO") {
    op_c = "%";
  }

  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", operation}}),
            "Value3 = Value0 " + op_c + " Value1;");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_ADD) {
  functionNodeIntegerMathBasicHelper("ADD");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_SUBTRACT) {
  functionNodeIntegerMathBasicHelper("SUBTRACT");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_MULTIPLY) {
  functionNodeIntegerMathBasicHelper("MULTIPLY");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_DIVIDE) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "DIVIDE"}}),
            "Value3 = (Value1 == 0 ? 0 : Value0 / Value1);");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_MODULO) {
  functionNodeIntegerMathBasicHelper("MODULO");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_POWER) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "POWER"}}),
            "Value3 = int(pow(float(Value0), float(Value1)));");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_MIN) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "MINIMUM"}}),
            "Value3 = min(Value0, Value1);");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_MAX) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "MAXIMUM"}}),
            "Value3 = max(Value0, Value1);");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_MULTIPLY_ADD) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::LEFT, "Value2"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "MULTIPLY_ADD"}}),
            "Value3 = (Value0 * Value1) + Value2;");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_ABSOLUTE) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "ABSOLUTE"}}),
            "Value3 = abs(Value0);");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_NEGATE) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "NEGATE"}}),
            "Value3 = -(Value0);");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_SIGN) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "SIGN"}}),
            "Value3 = int(sign(float(Value0)));");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_DIVIDE_ROUND) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {
                    {"operation0", "DIVIDE_ROUND"},
                }),
            "Value3 = int(round(float(Value0) / float(Value1)));");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_DIVIDE_FLOOR) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {
                    {"operation0", "DIVIDE_FLOOR"},
                }),
            "Value3 = int(floor(float(Value0) / float(Value1)));");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_DIVIDE_CEIL) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {
                    {"operation0", "DIVIDE_CEIL"},
                }),
            "Value3 = int(ceil(float(Value0) / float(Value1)));");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_FLOORED_MODULO) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {
                    {"operation0", "FLOORED_MODULO"},
                }),
            "Value3 = (Value1 == 0 ? 0 : Value0 - int(floor(float(Value0) / "
            "float(Value1))) * Value1;)");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_GCD) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "GCD"}}),
            "{int gcd_x = abs(Value0);int gcd_y = abs(Value1);while (gcd_y != "
            "0) {int gcd_tmp = gcd_x % gcd_y;gcd_x = gcd_y;gcd_y = "
            "gcd_tmp;}Value3 = gcd_x;}");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_INTEGER_MATH_LCM) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeIntegerMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "LCM"}}),
            "{int lcm_a = abs(Value0);int lcm_b = abs(Value1);int lcm_x = "
            "lcm_a;int lcm_y = lcm_b;while (lcm_y != 0) {int lcm_tmp = lcm_x % "
            "lcm_y;lcm_x = lcm_y;lcm_y = lcm_tmp;}Value3 = (lcm_x == 0) ? 0 : "
            "abs((lcm_a / lcm_x) * lcm_b);}");
}

//=========================================================================
// ShaderNodeMapRange
//=========================================================================

auto shaderNodeMapRangeHelper(std::string data_type, std::string interpolation,
                              bool clamp) {
  std::string val, fmin, fmax, tmin, tmax, steps, res, glsl_type;

  if (data_type == "FLOAT") {
    val = "Value0";
    fmin = "From Min0";
    fmax = "From Max0";
    tmin = "To Min0";
    tmax = "To Max0";
    steps = "Steps0";
    res = "Result0";
    glsl_type = "float";
  } else if (data_type == "FLOAT_VECTOR") {
    val = "Vector0";
    fmin = "From Min1";
    fmax = "From Max1";
    tmin = "To Min1";
    tmax = "To Max1";
    steps = "Steps1";
    res = "Vector0";
    glsl_type = "vec3";
  }

  // Building blocks of the expected GLSL
  const std::string factor =
      "(" + val + " - " + fmin + ") / (" + fmax + " - " + fmin + ")";
  const std::string to_range = "(" + tmax + " - " + tmin + ")";
  const std::string to_bounds =
      "min(" + tmin + ", " + tmax + "), max(" + tmin + ", " + tmax + "));";

  std::string expected;

  if (interpolation == "LINEAR") {
    std::string body = tmin + " + " + factor + " * " + to_range;
    expected = clamp ? res + " = clamp(" + body + ", " + to_bounds
                     : res + " = " + body + ";";
  } else if (interpolation == "STEPPED") {
    std::string body = tmin + " + floor(" + factor + " * (" + steps +
                       " + 1.0)) / " + steps + " * " + to_range;
    expected = clamp ? res + " = clamp(" + body + ", " + to_bounds
                     : res + " = " + body + ";";
  } else if (interpolation == "SMOOTHSTEP") {
    // The clamp flag has no effect here: the factor is always clamped to 0..1.
    expected = "{" + glsl_type + " t = clamp(" + factor + ", 0.0, 1.0);" + res +
               " = " + tmin + " + (t * t * (3.0 - 2.0 * t)) * " + to_range +
               ";}";
  } else if (interpolation == "SMOOTHERSTEP") {
    // The clamp flag has no effect here: the factor is always clamped to 0..1.
    expected = "{" + glsl_type + " t = clamp(" + factor + ", 0.0, 1.0);" + res +
               " = " + tmin +
               " + (t * t * t * (t * (t * 6.0 - 15.0) + 10.0)) * " + to_range +
               ";}";
  }

  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMapRange>(
                {
                    {GraphShim::LEFT, val},
                    {GraphShim::LEFT, fmin},
                    {GraphShim::LEFT, fmax},
                    {GraphShim::LEFT, tmin},
                    {GraphShim::LEFT, tmax},
                    {GraphShim::LEFT, steps},
                    {GraphShim::RIGHT, res},
                },
                {{"data_type0", data_type},
                 {"interpolation_type0", interpolation},
                 {"clamp0", clamp ? "True" : "False"}}),
            expected);
}

//============================== Float ====================================

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_LINEAR) {
  shaderNodeMapRangeHelper("FLOAT", "LINEAR", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_LINEAR_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT", "LINEAR", true);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_STEPPED) {
  shaderNodeMapRangeHelper("FLOAT", "STEPPED", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_STEPPED_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT", "STEPPED", true);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_SMOOTHSTEP) {
  shaderNodeMapRangeHelper("FLOAT", "SMOOTHSTEP", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_SMOOTHSTEP_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT", "SMOOTHSTEP", true);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_SMOOTHERSTEP) {
  shaderNodeMapRangeHelper("FLOAT", "SMOOTHERSTEP", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_FLOAT_SMOOTHERSTEP_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT", "SMOOTHERSTEP", true);
}

//============================== Vector ===================================

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_LINEAR) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "LINEAR", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_LINEAR_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "LINEAR", true);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_STEPPED) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "STEPPED", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_STEPPED_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "STEPPED", true);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_SMOOTHSTEP) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "SMOOTHSTEP", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_SMOOTHSTEP_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "SMOOTHSTEP", true);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_SMOOTHERSTEP) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "SMOOTHERSTEP", false);
}

TEST(BLENDER_MODULES, SHADER_NODE_MAP_RANGE_VECTOR_SMOOTHERSTEP_CLAMP) {
  shaderNodeMapRangeHelper("FLOAT_VECTOR", "SMOOTHERSTEP", true);
}
