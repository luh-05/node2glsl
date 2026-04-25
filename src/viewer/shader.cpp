#include "shader.h"
#include "spdlog/spdlog.h"
#include <fstream>
namespace ntg::viz {

std::vector<uint32_t> Shader::compileGLSLToSpv(const std::string &source,
                                               shaderc_shader_kind kind,
                                               const char *filename) {
  shaderc::Compiler compiler;
  shaderc::CompileOptions options;

  options.SetOptimizationLevel(shaderc_optimization_level_performance);

  shaderc::SpvCompilationResult module =
      compiler.CompileGlslToSpv(source, kind, filename, options);

  if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
    spdlog::error("Shaderc error: {}", module.GetErrorMessage());
    return std::vector<uint32_t>();
  }

  return {module.cbegin(), module.cend()};
}

auto read_file(std::string_view path) -> std::optional<std::string> {
  constexpr auto read_size = std::size_t(4096);
  auto stream = std::ifstream(path.data());
  stream.exceptions(std::ios_base::badbit);

  if (not stream) {
    return std::nullopt;
  }

  auto out = std::string();
  auto buf = std::string(read_size, '\0');
  while (stream.read(&buf[0], read_size)) {
    out.append(buf, 0, stream.gcount());
  }
  out.append(buf, 0, stream.gcount());
  return std::optional<std::string>{out};
}

bool Shader::loadShaderFromFile(SDL_GPUDevice *device, const char *path,
                                const char *name, Shader_Type type,
                                const ShaderAttribs *attribs) {
  spdlog::debug("Compiling shader '{}'", name);
  this->name = name;
  auto code = read_file(path);
  if (!code.has_value()) {
    spdlog::error("Failed to read shader '{}'!", name);
    return false;
  }
  spdlog::debug("Shader code: \n{}", code.value());

  SDL_GPUShaderCreateInfo create_info;

  std::vector<uint32_t> data;
  switch (type) {
  case SPIRV_VERTEX:
    data = compileGLSLToSpv(code.value(), shaderc_vertex_shader, path);
    create_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    create_info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    break;
  case SPIRV_FRAGMENT:
    data = compileGLSLToSpv(code.value(), shaderc_fragment_shader, name);
    create_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
    create_info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    break;
  _:
    spdlog::error("'{}' is not a valid shader type!",
                  static_cast<uint32_t>(type));
    return false;
  }

  create_info.code_size = data.size() * sizeof(uint32_t);
  create_info.code = reinterpret_cast<uint8_t *>(data.data());
  create_info.entrypoint = "main";
  create_info.num_samplers = attribs->num_samplers;
  create_info.num_storage_textures = attribs->num_storage_textures;
  create_info.num_storage_buffers = attribs->num_storage_buffers;
  create_info.num_uniform_buffers = attribs->num_uniform_buffers;

  SDL_GPUShader *sh = SDL_CreateGPUShader(device, &create_info);
  if (sh == nullptr) {
    spdlog::error("Couldn't compile shader '{}': {}", name, SDL_GetError());
    return false;
  }
  this->shader = sh;
  spdlog::debug("Successfully compiled shader '{}'", name);
  return true;
}

} // namespace ntg::viz
