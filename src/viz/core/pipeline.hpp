#include <memory>
#pragma ONCE

namespace ntg::viz {
class Shader;
struct PipelineTemplate {
  std::unique_ptr<Shader> vertex_shader;
  std::unique_ptr<Shader> fragment_shader;
};
class Pipeline {
private:
  std::unique_ptr<Shader> vertex_shader;
  std::unique_ptr<Shader> fragment_shader;

public:
  Pipeline();
  ~Pipeline();
};
} // namespace ntg::viz
