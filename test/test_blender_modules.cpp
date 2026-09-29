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

// auto shaderNodeMathHelper(std::string operation) {
//   EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
//                 {}, {{"operation0", operation}}),
//             "dflksdjflkj " + operation + " sadfalkjsdjfls");
// }

// TEST(BLENDER_MODULES, SHADER_NODE_MATH_ADD) { shaderNodeMathHelper("ADD"); }




//================================== BOOLEAN MATH =================================================

auto functionNodeBooleanMathHelper(std::string operation, std::string prefix, std::string infix, std::string suffix){
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeBooleanMath>(
    {
    {GraphShim::LEFT, "Boolean1"},
    {GraphShim::RIGHT, "Boolean2"},
    {GraphShim::LEFT, "Boolean0"},
    },
    {{"operation0", operation}}
  ),
  std::string ("Boolean2 = " ) + prefix + "Boolean0 " + infix + "Boolean1" + suffix 
);
}


TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_NOT) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeBooleanMath>(
    {
    {GraphShim::LEFT, "Boolean1"},
    {GraphShim::RIGHT, "Boolean2"},
    {GraphShim::LEFT, "Boolean0"},
    },
    {{"operation0", "NOT"}}
  ),
  std::string("Boolean2 = !") + "Boolean0;"
);
}


TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_AND) {
  
functionNodeBooleanMathHelper("AND", "", "&& ", ";");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_OR) {
  
functionNodeBooleanMathHelper("OR", "", "|| ", ";");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_NAND) {
  
functionNodeBooleanMathHelper("NAND", "!(", "&& ", ");");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_NOR) {
  
functionNodeBooleanMathHelper("NOR", "!(", "|| ", ");");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_XOR) {
  
functionNodeBooleanMathHelper("XOR", "", "!= ", ";");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_XNOR) {
  
functionNodeBooleanMathHelper("XNOR", "", "== ", ";");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_IMPLY) {
  
functionNodeBooleanMathHelper("IMPLY", "(!", "|| ", ");");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BOOLEAN_MATH_NIMPLY) {
  
functionNodeBooleanMathHelper("NIMPLY", "(", "&& !", ");");
}

//================================ BOOLEAN MATH ================================================



//================================= BIT MATH ========================================================
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
                {
                  {"operation0", "SHIFT"}
                }),
                  "if (Shift0 > 0) {Value0=A0<<Shift0;}else if (Shift0 < 0) {Value0=A0>>(-Shift0);}else {Value0=A0;}");
}

TEST(BLENDER_MODULES, FUNCTION_NODE_BIT_MATH_ROTATE) {
  EXPECT_EQ(testImpl<GenerateTokenStringFunctionNodeBitMath>(
                {
                    {GraphShim::LEFT, "A0"},
                    {GraphShim::LEFT, "B0"},
                    {GraphShim::LEFT, "Shift0"},
                    {GraphShim::RIGHT, "Value0"},
                },
                {
                  {"operation0", "ROTATE"}
                }),
                  "if (Shift0 > 0) {Value0=(A0<<Shift0) | (A0>> (32 - Shift0));}else if (Shift0 < 0) {Value0=(A0>> (-Shift0)) | (A0<< (32 + Shift0));}else {Value0=A0;}");
}
//================================= BIT MATH =============================================

//================================= CLAMP =========================================================

TEST(BLENDER_MODULES, SHADER_NODE_CLAMP_MINMAX) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeClamp>(
    {
      {GraphShim::RIGHT, "Result0"},
      {GraphShim::LEFT, "Value0"},
      {GraphShim::LEFT, "Min0"},
      {GraphShim::LEFT, "Max0"},
    },
    {
      {"clamp_type0", "MINMAX"}
    }),
    "Result0 = clamp(Value0, Min0, Max0);"
);
}
//TODO: Hier sind irgendwie 3 Outputs noch Value0

TEST(BLENDER_MODULES, SHADER_NODE_CLAMP_RANGE){
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeClamp>(
    {
      {GraphShim::RIGHT, "Result0"},
      {GraphShim::LEFT, "Value0"},
      {GraphShim::LEFT, "Min0"},
      {GraphShim::LEFT, "Max0"}
        }, 
        {
          {"clamp_type0", "RANGE"}
        }),
          "Result0 = clamp(Value0, min(Min0, Max0), max(Min0, Max0));");
}
//====================== END CLAMP ========================

//============= START SHADER NODE MATH ======================
// ============== FIRST BLOCK ========================
auto shaderNodeMathHelper1(std::string operation, std::string clamp) {
  std::string sign;
  if(operation == "ADD"){
    sign = "+";
  }
  else if (operation == "SUBTRACT"){
    sign = "-";
  }
  else if (operation == "MULTIPLY"){
    sign = "*";
  }
  else if (operation == "DIVIDE") {
    sign = "/";
  }
  else if (operation == "LESS_THAN") {
    sign = "<";
  }
  else if (operation == "GREATER_THAN") {
    sign = ">";
  }
if(clamp == "True"){
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {
                  {"operation0", operation}, {"use_clamp0", clamp}
              }
            ),
            "Value3=clamp(Value0" + sign + "Value1, 0.0, 1.0);");
}

else if(clamp!="True"){ EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", operation}, {"use_clamp0", clamp}}),
            "Value3=Value0" + sign + "Value1;");
}
}

// ADD
TEST(BLENDER_MODULES, SHADER_NODE_MATH_ADD_NOCLAMP) {
  shaderNodeMathHelper1("ADD", "False");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_ADD_CLAMP) {
  shaderNodeMathHelper1("ADD", "True");
}

// SUBTRACT
TEST(BLENDER_MODULES, SHADER_NODE_MATH_SUBTRACT_NOCLAMP) {
  shaderNodeMathHelper1("SUBTRACT", "False");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_SUBTRACT_CLAMP) {
  shaderNodeMathHelper1("SUBTRACT", "True");
}

// MULTIPLY
TEST(BLENDER_MODULES, SHADER_NODE_MATH_MULTIPLY_NOCLAMP) {
  shaderNodeMathHelper1("MULTIPLY", "False");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_MULTIPLY_CLAMP) {
  shaderNodeMathHelper1("MULTIPLY", "True");
}

// DIVIDE
TEST(BLENDER_MODULES, SHADER_NODE_MATH_DIVIDE_NOCLAMP) {
  shaderNodeMathHelper1("DIVIDE", "False");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_DIVIDE_CLAMP) {
  shaderNodeMathHelper1("DIVIDE", "True");
}

// LESS_THAN
TEST(BLENDER_MODULES, SHADER_NODE_MATH_LESS_THAN_NOCLAMP) {
  shaderNodeMathHelper1("LESS_THAN", "False");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_LESS_THAN_CLAMP) {
  shaderNodeMathHelper1("LESS_THAN", "True");
}

// GREATER_THAN
TEST(BLENDER_MODULES, SHADER_NODE_MATH_GREATER_THAN_NOCLAMP) {
  shaderNodeMathHelper1("GREATER_THAN", "False");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_GREATER_THAN_CLAMP) {
  shaderNodeMathHelper1("GREATER_THAN", "True");
}
// ============ END OF FIRST BLOCK =======================



// =============== SECOND BLOCK (one function, one variable)======================

auto shaderNodeMathHelper2(std::string operation, std::string clamp) {
  std::string function;
  if (operation == "SQRT") {
    function = "sqrt(";
  } else if (operation == "INVERSE_SQRT") {
    function = "inversesqrt(";
  } else if (operation == "EXPONENT") {
    function = "pow(2.718281828459045, ";
  } else if (operation == "ABSOLUTE") {
    function = "abs(";
  } else if (operation == "FLOOR") {
    function = "floor(";
  } else if (operation == "SIGN") {
    function = "sign(";
  } else if (operation == "CEIL") {
    function = "ceil(";
  } else if (operation == "FRACT") {
    function = "fract(";
  } else if (operation == "TRUNC") {
    function = "trunc(";
  } else if (operation == "ROUND") {
    function = "round(";
  } else if (operation == "SINE") {
    function = "sin(";
  } else if (operation == "COSINE") {
    function = "cos(";
  } else if (operation == "TANGENT") {
    function = "tan(";
  } else if (operation == "ARCSINE") {
    function = "asin(";
  } else if (operation == "ARCCOSINE") {
    function = "acos(";
  } else if (operation == "ARCTANGENT") {
    function = "atan(";
  } else if (operation == "SINH") {
    function = "sinh(";
  } else if (operation == "COSH") {
    function = "cosh(";
  } else if (operation == "TANH") {
    function = "tanh(";
  } else if (operation == "RADIANS") {
    function = "radians(";
  }

  if (clamp == "True") {
    EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                  {
                      {GraphShim::LEFT, "Value0"},
                      {GraphShim::RIGHT, "Value3"},
                  },
                  {{"operation0", operation}, {"use_clamp0", clamp}}),
                "Value3=clamp(" + function + "Value0), 0.0, 1.0);");
  } 
  
  else if (clamp != "True") {
    EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                  {
                      {GraphShim::LEFT, "Value0"},
                      {GraphShim::RIGHT, "Value3"},
                  },
                  {{"operation0", operation}, {"use_clamp0", clamp}}),
                "Value3=" + function + "Value0);");
  }
}

  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SQRT_NOCLAMP) {
    shaderNodeMathHelper2("SQRT", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SQRT_CLAMP) {
    shaderNodeMathHelper2("SQRT", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_INVERSE_SQRT_NOCLAMP) {
    shaderNodeMathHelper2("INVERSE_SQRT", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_INVERSE_SQRT_CLAMP) {
    shaderNodeMathHelper2("INVERSE_SQRT", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_EXPONENT_NOCLAMP) {
    shaderNodeMathHelper2("EXPONENT", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_EXPONENT_CLAMP) {
    shaderNodeMathHelper2("EXPONENT", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ABSOLUTE_NOCLAMP) {
    shaderNodeMathHelper2("ABSOLUTE", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ABSOLUTE_CLAMP) {
    shaderNodeMathHelper2("ABSOLUTE", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_FLOOR_NOCLAMP) {
    shaderNodeMathHelper2("FLOOR", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_FLOOR_CLAMP) {
    shaderNodeMathHelper2("FLOOR", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SIGN_NOCLAMP) {
    shaderNodeMathHelper2("SIGN", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SIGN_CLAMP) {
    shaderNodeMathHelper2("SIGN", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_CEIL_NOCLAMP) {
    shaderNodeMathHelper2("CEIL", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_CEIL_CLAMP) {
    shaderNodeMathHelper2("CEIL", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_FRACT_NOCLAMP) {
    shaderNodeMathHelper2("FRACT", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_FRACT_CLAMP) {
    shaderNodeMathHelper2("FRACT", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_TRUNC_NOCLAMP) {
    shaderNodeMathHelper2("TRUNC", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_TRUNC_CLAMP) {
    shaderNodeMathHelper2("TRUNC", "True");
  } 
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ROUND_NOCLAMP) {
    shaderNodeMathHelper2("ROUND", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ROUND_CLAMP) {
    shaderNodeMathHelper2("ROUND", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SINE_NOCLAMP) {
    shaderNodeMathHelper2("SINE", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SINE_CLAMP) {
    shaderNodeMathHelper2("SINE", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_COSINE_NOCLAMP) {
    shaderNodeMathHelper2("COSINE", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_COSINE_CLAMP) {
    shaderNodeMathHelper2("COSINE", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_TANGENT_NOCLAMP) {
    shaderNodeMathHelper2("TANGENT", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_TANGENT_CLAMP) {
    shaderNodeMathHelper2("TANGENT", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCSINE_NOCLAMP) {
    shaderNodeMathHelper2("ARCSINE", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCSINE_CLAMP) {
    shaderNodeMathHelper2("ARCSINE", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCCOSINE_NOCLAMP) {
    shaderNodeMathHelper2("ARCCOSINE", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCCOSINE_CLAMP) {
    shaderNodeMathHelper2("ARCCOSINE", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCTANGENT_NOCLAMP) {
    shaderNodeMathHelper2("ARCTANGENT", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCTANGENT_CLAMP) {
    shaderNodeMathHelper2("ARCTANGENT", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SINH_NOCLAMP) {
    shaderNodeMathHelper2("SINH", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_SINH_CLAMP) {
    shaderNodeMathHelper2("SINH", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_COSH_NOCLAMP) {
    shaderNodeMathHelper2("COSH", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_COSH_CLAMP) {
    shaderNodeMathHelper2("COSH", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_TANH_NOCLAMP) {
    shaderNodeMathHelper2("TANH", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_TANH_CLAMP) {
    shaderNodeMathHelper2("TANH", "True");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_RADIANS_NOCLAMP) {
    shaderNodeMathHelper2("RADIANS", "False");
  }
  TEST(BLENDER_MODULES, SHADER_NODE_MATH_RADIANS_CLAMP) {
    shaderNodeMathHelper2("RADIANS", "True");
  }



 // ======================= END OF SECOND BLOCK =======================

// ======================= THIRD BLOCK (one function, two variables)======================  
auto shaderNodeMathHelper3(std::string operation, std::string clamp) {
  std::string function;
  if (operation == "POWER") {
    function = "pow(";
  } else if (operation == "MINIMUM") {
    function = "min(";
  } else if (operation == "MAXIMUM") {
    function = "max(";
  } else if (operation == "ARCTAN2") {
    function = "atan(";
  }

  if (clamp == "True") {
    EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                  {
                      {GraphShim::LEFT, "Value0"},
                      {GraphShim::LEFT, "Value1"},
                      {GraphShim::RIGHT, "Value3"},
                  },
                  {{"operation0", operation}, {"use_clamp0", clamp}}),

                "Value3=clamp(" + function + "Value0, Value1), 0.0, 1.0);");
  } 
  
  
  else if (clamp != "True") {
    EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                  {
                      {GraphShim::LEFT, "Value0"},
                      {GraphShim::LEFT, "Value1"},
                      {GraphShim::RIGHT, "Value3"},
                  },
                  {{"operation0", operation}, {"use_clamp0", clamp}}),
                "Value3=" + function + "Value0, Value1);");
  }
}


TEST(BLENDER_MODULES, SHADER_NODE_MATH_POWER_NOCLAMP) {
  shaderNodeMathHelper3("POWER", "False");
}
TEST(BLENDER_MODULES, SHADER_NODE_MATH_POWER_CLAMP) {
  shaderNodeMathHelper3("POWER", "True");
}
TEST(BLENDER_MODULES, SHADER_NODE_MATH_MINIMUM_NOCLAMP) {
  shaderNodeMathHelper3("MINIMUM", "False");
}
TEST(BLENDER_MODULES, SHADER_NODE_MATH_MINIMUM_CLAMP) {
  shaderNodeMathHelper3("MINIMUM", "True");
}
TEST(BLENDER_MODULES, SHADER_NODE_MATH_MAXIMUM_NOCLAMP) {
  shaderNodeMathHelper3("MAXIMUM", "False");
}
TEST(BLENDER_MODULES, SHADER_NODE_MATH_MAXIMUM_CLAMP) { 
  shaderNodeMathHelper3("MAXIMUM", "True");
}
TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCTAN2_NOCLAMP) {
  shaderNodeMathHelper3("ARCTAN2", "False");
}
TEST(BLENDER_MODULES, SHADER_NODE_MATH_ARCTAN2_CLAMP) {
  shaderNodeMathHelper3("ARCTAN2", "True");
} 
// ===================== END OF THIRD BLOCK =======================

// ===================== FOURTH BLOCK (all different) =======================
TEST(BLENDER_MODULES, SHADER_NODE_MATH_LOGARITHM_NOCLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "LOGARITHM"},{"use_clamp0", "False"}}),

            "Value3=log(Value0) / log(Value1);");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_LOGARITHM_CLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "LOGARITHM"},{"use_clamp0", "True"}}),

            "Value3=clamp(log(Value0) / log(Value1), 0.0, 1.0);");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_COMPARE_NOCLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::LEFT, "Value2"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "COMPARE"},{"use_clamp0", "False"}}),

        "if(abs(Value0-Value1) <= Value2) {Value3=1.0;} else {Value3=0.0;}") ;
}


TEST(BLENDER_MODULES, SHADER_NODE_MATH_COMPARE_CLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::LEFT, "Value2"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "COMPARE"},{"use_clamp0", "True"}}),
            "if(abs(Value0-Value1) <= Value2) {Value3=clamp(1.0, 0.0, 1.0);} else {Value3=clamp(0.0, 0.0, 1.0);}"
            );
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_MULTIPLY_ADD_NOCLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::LEFT, "Value2"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "MULTIPLY_ADD"},{"use_clamp0", "False"}}),

            "Value3=Value0*Value1+Value2;");
}

TEST(BLENDER_MODULES, SHADER_NODE_MATH_MULTIPLY_ADD_CLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMath>(
                {
                    {GraphShim::LEFT, "Value0"},
                    {GraphShim::LEFT, "Value1"},
                    {GraphShim::LEFT, "Value2"},
                    {GraphShim::RIGHT, "Value3"},
                },
                {{"operation0", "MULTIPLY_ADD"},{"use_clamp0", "True"}}),

            "Value3=clamp(Value0*Value1+Value2, 0.0, 1.0);");
}

// ===================== END OF FOURTH BLOCK =======================



//====================== END SHADER NODE MATH =================

//===================== SHADER NODE MIX ==========================
TEST(BLENDER_MODULES, SHADER_NODE_MIX_FLOAT_CLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMix>(
    {
      {GraphShim::LEFT, "A0"},
      {GraphShim::LEFT, "B0"},
      {GraphShim::LEFT, "Factor0"},
      {GraphShim::RIGHT, "Result0"},

    },
    {{"data_type0", "FLOAT"}, {"clamp_factor0", "True"}}),
    "Result0=mix(A0, B0, clamp(Factor0, 0.0, 1.0));");
  
}

TEST(BLENDER_MODULES, SHADER_NODE_MIX_FLOAT_NOCLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMix>(
    {
      {GraphShim::LEFT, "A0"},
      {GraphShim::LEFT, "B0"},
      {GraphShim::LEFT, "Factor0"},
      {GraphShim::RIGHT, "Result0"},

    },
    {{"data_type0", "FLOAT"}, {"clamp_factor0", "False"}}),
    "Result0=mix(A0, B0, Factor0);");
  
}

TEST(BLENDER_MODULES, SHADER_NODE_MIX_VECTOR_CLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMix>(
    {
      {GraphShim::LEFT, "A1"},
      {GraphShim::LEFT, "B1"},
      {GraphShim::LEFT, "Factor0"},
      {GraphShim::RIGHT, "Result0"},

    },
    {{"data_type0", "VECTOR"}, {"clamp_factor0", "True"}, {"factor_mode0", "UNIFORM"}}),
    "Result0=mix(A1, B1, clamp(Factor0, 0.0, 1.0));");
  
}

//Uniform / Non Uniform makes no difference

TEST(BLENDER_MODULES, SHADER_NODE_MIX_VECTOR_NOCLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMix>(
    {
      {GraphShim::LEFT, "A1"},
      {GraphShim::LEFT, "B1"},
      {GraphShim::LEFT, "Factor0"},
      {GraphShim::RIGHT, "Result0"},

    },
    {{"data_type0", "VECTOR"}, {"clamp_factor0", "False"}, {"factor_mode0", "UNIFORM"}}),
    "Result0=mix(A1, B1, Factor0);");
  
}


//TODO: NON UNIFORM


TEST(BLENDER_MODULES, SHADER_NODE_MIX_ROTATION_CLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMix>(
    {
      {GraphShim::LEFT, "A3"},
      {GraphShim::LEFT, "B3"},
      {GraphShim::LEFT, "Factor0"},
      {GraphShim::RIGHT, "Result0"},

    },
    {{"data_type0", "ROTATION"}, {"clamp_factor0", "True"}}),
    "Result0=mix(A3, B3, clamp(Factor0, 0.0, 1.0));");
  
}

TEST(BLENDER_MODULES, SHADER_NODE_MIX_ROTATION_NOCLAMP) {
  EXPECT_EQ(testImpl<GenerateTokenStringShaderNodeMix>(
    {
      {GraphShim::LEFT, "A3"},
      {GraphShim::LEFT, "B3"},
      {GraphShim::LEFT, "Factor0"},
      {GraphShim::RIGHT, "Result0"},

    },
    {{"data_type0", "ROTATION"}, {"clamp_factor0", "False"}}),
    "Result0=mix(A3, B3, Factor0);");
  
}
//======================= END SHADER NODE MIX ======================

