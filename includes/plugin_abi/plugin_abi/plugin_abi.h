
#ifndef PLUGIN_ABI_H
#define PLUGIN_ABI_H

#include <stdbool.h>
#include <stdint.h>

#define PLUGIN_ID_MAX 64
#define PLUGIN_NAME_MAX 128
#define PLUGIN_ERROR_MAX 256

#ifdef __cplusplus

extern "C" {

#endif

typedef struct {
  uint32_t errc;
  char message[PLUGIN_ERROR_MAX];
} PluginStatus;

typedef void (*ModuleFunc)(void *out, PluginStatus *status);

typedef bool (*PluginModuleCallback)(const char *name, ModuleFunc func,
                                     void *userdata);

bool EnumerateModules(PluginModuleCallback callback, void *userdata);

typedef struct {
  char id[PLUGIN_ID_MAX];
  char name[PLUGIN_NAME_MAX];
} PluginInfo;

void GetInfo(PluginInfo *info);
#ifdef __cplusplus
}
#endif

#endif
