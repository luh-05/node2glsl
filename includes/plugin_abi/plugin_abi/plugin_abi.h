
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

typedef enum : int32_t {
  SUCCESS = 0,  // Successs
  FAILURE = -1, // Generic/internal failure

  VERSION_MISMATCH = 1,     // Version mismatch, likely compatible
  VERSION_INCOMPATIBLE = 2, // Version incompatible
} ERR;

/// == HOST ==

// Shim struct to pass absl::Status
typedef struct {
  uint32_t errc;
  char message[PLUGIN_ERROR_MAX];
} PluginStatus;

// Pointer type for Module Implementations
typedef void (*ModuleFunc)(void *out, PluginStatus *status);

typedef struct {
  void *context;

  // Registers new Implementation
  ERR (*register_impl)(void *context, const char *name, ModuleFunc func);

  // Gets implementation or nullptr
  void *(*get_impl)(void *context, const char *name);
} HostModuleAPI;

typedef struct {
  void *context;

  // Register new constant under specified name and Node
  ERR (*register_const)(void *context, void *node, const char *name,
                        const char *value);
} HostConstantAPI;

typedef enum : uint8_t { LEFT = 0, RIGHT = 1 } PortPolarity;
typedef struct {
  void *context;

  ERR (*add_graph)(void *context, void *graph, void **new_graph,
                   const char *name);
  ERR (*add_module)(void *context, void *graph, void **module,
                    const char *name);
  ERR (*add_port)(void *context, void *node, void **port, PortPolarity p,
                  const char *name, const char *type);
  ERR (*add_connection)(void *context, void *port_l, void *port_r);
  ERR (*get_graph)(void *context, void **graph, const char *name);
  ERR (*get_module)(void *context, void **module, const char *name);
  ERR (*get_port)(void *context, void **port, const char *name);
} HostGraphAPI;

typedef struct {
  const char *from;
  const char *to;
  const char *pattern;
} ImplicitCastPolicy;
typedef struct {
  void *context;

  ERR (*register_definition)(void *context, const char *def);
  ERR (*register_implicit_cast)(void *context, ImplicitCastPolicy *cast);
} HostTypeAPI;

typedef struct {
  uint32_t version_major;
  uint32_t version_minor;

  const HostModuleAPI *modules;
  const HostConstantAPI *constants;
  const HostGraphAPI *graph;
  const HostTypeAPI *type;
} HostAPI;

// == PLUGIN ==

// Struct containing extra arguments if a plugin needs data beyond the scope of
// the API
typedef struct {
  void *userdata;

  ERR (*get_uint32)(const char *key, uint32_t **val);
  ERR (*get_string)(const char *key, char **val);
} CallContext;

typedef struct {
  uint32_t version_major;
  uint32_t version_minor;

  void *instance; // internal plugin state

  // Registers all implementations provided by the plugin
  ERR (*register_implementations)(void *instance);
  // Parses Graph into provided graph object
  ERR (*parse_graph)(void *instance, CallContext *cc, void *graph);

  // Shut down plugin
  ERR (*shutdown)(void *instance);
} PluginAPI;

// Entry point
ERR PluginInit(const HostAPI *host, PluginAPI *plugin);

// == LEGACY, WILL BE REMOVED SOON ==

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
