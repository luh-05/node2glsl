
#include <mir/codegen.hpp>
#include <mir/node_graph/node_graph.hpp>
namespace msk::ir {
using Out = Module::Out;

void Out::FlushBuffer() {
  *this->it = std::make_unique<TextToken>(text_buff);
  text_buff = "";
}

Out &Out::operator+(Out::PortFetch fetch) {
  auto [p, name] = fetch;

  auto *map = &this->parent.leftPorts;
  if (Polarity::RIGHT == p)
    map = &this->parent.rightPorts;

  if (!map->contains(name)) {
    // FIXME: Ports not implemented correctly yet, so this will always throw
    // if (this->status.ok()) {
    //   this->status = absl::NotFoundError(
    //       std::format("Cound not find {} token '{}'",
    //                   p == RIGHT ? "right" : "left", name));
    // }
    // return *this;
  }
  FlushBuffer();

  if (this->status.ok()) {
    *(this->it) = std::make_unique<WildcardToken>(map->operator[](name).get());
  }

  return *this;
}

} // namespace msk::ir
