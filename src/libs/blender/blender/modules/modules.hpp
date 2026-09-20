#include <absl/status/status.h>
#include <mir/node_graph/node_graph.hpp>
#pragma once

namespace msk::blender {

using Out = msk::ir::Module::Out;
/**
 *  @brief Dummy Module
 */
auto GenerateTokenStringDummy(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeBitMath(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeBooleanMath(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeCompare(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeHashValue(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeIntegerMath(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeFloatToInt(Out &out) -> absl::Status;
auto GenerateTokenStringNodeGroupOutput(Out &out) -> absl::Status;

auto GenerateTokenStringShaderNodeClamp(Out &out) -> absl::Status;

auto GenerateTokenStringShaderNodeFloatCurve(Out &out) -> absl::Status;

auto GenerateTokenStringShaderNodeMapRange(Out &out) -> absl::Status;

auto GenerateTokenStringShaderNodeMath(Out &out) -> absl::Status;

auto GenerateTokenStringShaderNodeMix(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeInputBool(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeInputInt(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeInputRotation(Out &out) -> absl::Status;

auto GenerateTokenStringFunctionNodeInputVector(Out &out) -> absl::Status;

auto GenerateTokenStringShaderNodeValue(Out &out) -> absl::Status;
} // namespace msk::blender
