
#ifndef LOGGER_USER_PLUGIN_API
#define LOGGER_USER_PLUGIN_API

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

struct logger_user_keyvalue_type {
  const char* key;
  const char* value;
};

struct logger_user_command_plugin_callbacks_type {
  void*(*plugin_create_callback)();
  void(*plugin_destroy_callback)(void* plugin_obj);
  void(*plugin_config_updated_callback)(void* plugin_obj, const logger_user_keyvalue_type* config_data);
  
  char*(*plugin_cmd_callback)(void* plugin_obj, int cmd_id, int verb_level, void* addr, const void* vparam, int iparam);
  void(*plugin_free_buffer_callback)(void* plugin_obj, char* str);

};

void* logger_create_user_command_plugin_factory(const char* name, const char* type_name, int* cmds_ids, int cmds_ids_length,
  const struct logger_user_command_plugin_callbacks_type* callbacks);

#ifdef __cplusplus
};
#endif /*__cplusplus*/

#endif /*LOGGER_USER_PLUGIN_API*/
