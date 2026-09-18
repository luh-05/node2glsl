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

class FunctionNodeBitMath : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeBooleanMath : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeCompare : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeHashValue : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeIntegerMath : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeFloatToIntMath : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeClamp : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeFloatCurve : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeMapRange : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeMath : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeMix : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeInputBool : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeInputInt : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeInputRotation : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class FunctionNodeInputVector : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};

class ShaderNodeValue : public ir::Module {
public:
  using Module::Module;
  auto GenerateTokenString(Out &&out) -> absl::Status;
};
} // namespace msk::blender
