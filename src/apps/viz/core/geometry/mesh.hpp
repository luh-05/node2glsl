#pragma ONCE
#include "pch.hpp"
#include <vector>

namespace ntg::viz {
struct Vertex {
  glm::vec2 pos;
  glm::vec3 color;
};

class Mesh {
public:
  Mesh();
  ~Mesh();

public:
  std::vector<Vertex> vertices;
};
} // namespace ntg::viz
