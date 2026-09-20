
#include <memory>
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>
#include <spdlog/spdlog.h>

namespace msk::ir {
using Out = Module::Out;

void Out::FlushBuffer() {
  *this->it = TextToken(this->text_buff);
  this->text_buff = "";
}

bool Out::checkPort(Out::Polarity p, std::string name, Node::MapType *&map) {
  map = &this->parent.leftPorts;
  if (Polarity::RIGHT == p)
    map = &this->parent.rightPorts;

  name | std::views::transform([](unsigned char c) { return std::tolower(c); });

  if (!map->contains(name)) {
    // FIXME: Ports not implemented correctly yet, so this will always throw
    if (this->status.ok()) {
      this->status = absl::NotFoundError(
          std::format("Cound not find {} port '{}' (module: '{}')",
                      p == RIGHT ? "right" : "left", name,
                      reinterpret_cast<void *>(&this->parent)));
    }
    return false;
  }

  return true;
}
// auto Out::createWildcardToken(std::string_view name, Node::MapType *&map)
//     -> std::unique_ptr<CodegenToken> {
//   return std::make_unique<WildcardToken>(
//       map->operator[](std::string(name)).get());
// }

Out &Out::operator+(Out::PortFetch fetch) {
  auto [p, name] = fetch;

  Node::MapType *map;
  std::string name_lower = std::string(name);
  if (!this->checkPort(p, name_lower, map)) {
    return *this;
  }

  this->FlushBuffer();

  if (this->status.ok()) {
    auto port = map->operator[](name_lower).get();
    this->GetContext()->LogPort(&this->parent, port, p == Polarity::RIGHT);
    *this->it = WildcardToken(port);
  }

  return *this;
}

void Out::Legacy::AddTokenVector(std::vector<std::shared_ptr<MockToken>> &&v) {
  if (!self.status.ok())
    return;

  self.FlushBuffer();

  for (int i = 0; i < v.size(); i++) {
    auto *m = v[i].get();
    if (auto t = dynamic_cast<MTextToken *>(m)) {
      *self.it = TextToken(t->text);
    } else if (auto w = dynamic_cast<MWildcardToken *>(m)) {
      Node::MapType *map;
      std::string name_lower = std::string(w->name);
      if (!self.checkPort(w->p, name_lower, map)) {
        return;
      }

      auto port = map->operator[](name_lower).get();
      *self.it = WildcardToken(port);
    } else
      spdlog::warn("MockToken of base type discarded - how did this get here?");
  }
}
} // namespace msk::ir
