#ifndef __JERRY_HELPER_H
#define __JERRY_HELPER_H

#include "jerryscript.h"

typedef enum __PSJErrorCode {
    OK = 0x0,
    UNKNOWN_ERROR,
    MISSING_ARG,
    UNKNOWN_CMD,
    WRITE_ERROR,
    READ_ERROR,
    FILE_NOT_FOUND,
    INVALID_ARG,
    JAVASCRIPT_ERROR
} PSJErrorCode;

jerry_char_t *S(const jerry_char_t *txt);

jerry_char_t *VS(const jerry_char_t *format,va_list args);

bool psj_jerry_has_property(const jerry_value_t jObject, const jerry_char_t *property);

jerry_value_t psj_jerry_get_property(const jerry_value_t jObject, const jerry_char_t *property);

jerry_char_t *psj_jerry_get_string_property(const jerry_value_t jObject, const jerry_char_t *property);

uint32_t psj_jerry_get_uint32_property(const jerry_value_t jObject, const jerry_char_t *property);

void psj_jerry_set_property(const jerry_value_t jObject, const jerry_char_t *property, jerry_value_t value);

void psj_jerry_set_string_property(const jerry_value_t jObject, const jerry_char_t *property, const jerry_char_t *value);

void psj_jerry_set_uint32_property(const jerry_value_t jObject, const jerry_char_t *property, const uint32_t value);

jerry_char_t *psj_jerry_stringify(const jerry_value_t jObject);

jerry_char_t *psj_jerry_get_error_message(const PSJErrorCode error_code, ...);

jerry_value_t psj_jerry_create_error_obj(const PSJErrorCode error_code, ...);

jerry_value_t psj_jerry_exception_to_error_obj(jerry_value_t exception);

#endif // __JERRY_HELPER_H