#include "psj.h"

#define MAX_CONFIG_SIZE 1024

jerry_value_t config_obj = 0;

void init_configuration()
{
    // Max config size of 1kb
    jerry_char_t *jsonString = calloc(sizeof(jerry_char_t), MAX_CONFIG_SIZE);
    int32_t length = psj_flash_read_all(".config", jsonString, MAX_CONFIG_SIZE);

    if (length < 0)
    {
        config_obj = jerry_create_object();
    }
    else
    {
        config_obj = jerry_json_parse(jsonString, length);
    }

cleanup:
    if (jsonString != NULL) free(jsonString);
}

void cleanup_configuration()
{
    if (config_obj != 0)
    {
        jerry_release_value(config_obj);
        config_obj = 0;
    }
}

void save_configuration()
{
    jerry_char_t *jsonString = psj_jerry_stringify(config_obj);
    psj_flash_save(".config", jsonString, false);

cleanup:
    if (jsonString != NULL) free(jsonString);
}

jerry_char_t *config_get_autorun()
{
    return psj_jerry_get_string_property(config_obj, "autorun");
}

void config_set_autorun(jerry_char_t *path)
{
    psj_jerry_set_string_property(config_obj, "autorun", path);
    save_configuration();
}
