
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

// Shim struct to pass absl::Status errors
typedef struct {
  uint32_t errc;
  char message[PLUGIN_ERROR_MAX];
} PluginStatus;

// Pointer type for Module Implementations
typedef void (*ModuleFunc)(void *out, PluginStatus *status);

// Callback for Module Implementation registration
typedef bool (*PluginModuleCallback)(const char *name, ModuleFunc func,
                                     void *userdata);

/** @brief Enumerates all Module implementations sequentially to caller via
 * callback
 * @param callback - callback that passes name and implementation
 * @param userdata - structure type to be passed to the callback (e.g. std::map)
 */
bool EnumerateModules(PluginModuleCallback callback, void *userdata);

// Callback for Definition registeration
typedef void (*PluginDefinitionCallback)(const char *def, void *userdata);

/** @brief Enumerates all Precompiler Definitions sequentially to caller via
 * callback
 * @param callback - callback that passes definition
 * @param userdata - structure type to be passed to the callback (e.g.
 * std::vector)
 */
void EnumerateDefinitions(PluginDefinitionCallback callback, void *userdata);

// Struct containing plugin metadata
typedef struct {
  char id[PLUGIN_ID_MAX];
  char name[PLUGIN_NAME_MAX];
} PluginInfo;

// Fills in a PluginInfo struct
void GetInfo(PluginInfo *info);
#ifdef __cplusplus
}
#endif

#endif
