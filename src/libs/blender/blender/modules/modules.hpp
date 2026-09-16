#include <absl/status/status.h>
#include <mir/node_graph/node_graph.hpp>
#pragma once

namespace msk::blender {
/**
 *  @brief Dummy Module
 */
class DummyModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeBitMathModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeCompareModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeHashValueModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionIntegerMathModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeFloatToIntMathModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeBooleanMathModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeClampModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeFloatCurveModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeMapRangeModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeMathModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderMathMixModule : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

} // namespace msk::blender
