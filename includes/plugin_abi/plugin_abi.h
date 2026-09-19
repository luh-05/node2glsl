
#ifndef PLUGIN_ABI_H
#define PLUGIN_ABI_H

#include <stddef.h>

#ifdef __cplusplus

extern "C" {

#endif

typedef size_t (*ModuleFunc)(void *out);

typedef struct ModuleImpl {
  const char *name;
  ModuleFunc impl;
} ModuleImpl;

typedef void (*PluginModuleCallback)(const char *name, ModuleFunc func,
                                     void *userdata);

void EnumerateModules(PluginModuleCallback callback, void *userdata);

#ifdef __cplusplus
}
#endif

#endif
