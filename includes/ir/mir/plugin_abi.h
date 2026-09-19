
#ifndef PLUGIN_ABI_H
#define PLUGIN_ABI_H

#include <stddef.h>

#ifdef __cplusplus

namespace msk::ir {
extern "C" {

#endif

struct PluginFetchRequest {
  size_t module_count;
  // size_t type_resolver_count;
};

struct PluginInfo {
  void *module_names; // std::vector<std::string>
  // void * type_resolver;
};

#ifdef __cplusplus
}
} // namespace msk::ir
#endif

#endif
