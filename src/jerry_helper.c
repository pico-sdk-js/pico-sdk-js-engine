#include "jerry_helper.h"

#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

bool psj_jerry_has_property(const jerry_value_t jObject, const jerry_char_t *property)
{
    assert(jerry_value_is_object(jObject));

    jerry_value_t propertyValue = jerry_create_string(property);
    jerry_value_t hasPropValue = jerry_has_property(jObject, propertyValue);

    bool hasProp = jerry_get_boolean_value(hasPropValue);

    jerry_release_value(propertyValue);
    jerry_release_value(hasPropValue);

    return hasProp;
}

jerry_char_t *psj_jerry_get_string_property(const jerry_value_t jObject, const jerry_char_t *property)
{
    jerry_value_t value = psj_jerry_get_property(jObject, property);
    jerry_char_t *str_buf_p = psj_jerry_to_string(value);

    jerry_release_value(value);
    return str_buf_p;
}

jerry_char_t *psj_jerry_to_string(const jerry_value_t value)
{
    jerry_char_t *str_buf_p = NULL;

    if (jerry_value_is_string(value))
    {
        jerry_size_t req_sz = jerry_get_utf8_string_size(value);
        str_buf_p = malloc(sizeof(jerry_char_t) * (req_sz+1));
        jerry_string_to_utf8_char_buffer(value, str_buf_p, req_sz);
        str_buf_p[req_sz] = 0;
    }

    return str_buf_p;
}

uint psj_jerry_to_uint(const jerry_value_t value)
{
    return (uint)jerry_get_number_value(value);    
}

uint8_t psj_jerry_to_uint8_t(const jerry_value_t value)
{
    return (uint8_t)jerry_get_number_value(value);    
}

uint16_t psj_jerry_to_uint16_t(const jerry_value_t value)
{
    return (uint16_t)jerry_get_number_value(value);    
}

uint32_t psj_jerry_to_uint32_t(const jerry_value_t value)
{
    return (uint32_t)jerry_get_number_value(value);    
}

uint64_t psj_jerry_to_uint64_t(const jerry_value_t value)
{
    return (uint64_t)jerry_get_number_value(value);    
}

int psj_jerry_to_int(const jerry_value_t value)
{
    return (int)jerry_get_number_value(value);    
}

int8_t psj_jerry_to_int8_t(const jerry_value_t value)
{
    return (int8_t)jerry_get_number_value(value);    
}

int16_t psj_jerry_to_int16_t(const jerry_value_t value)
{
    return (int16_t)jerry_get_number_value(value);    
}

int32_t psj_jerry_to_int32_t(const jerry_value_t value)
{
    return (int32_t)jerry_get_number_value(value);    
}

int64_t psj_jerry_to_int64_t(const jerry_value_t value)
{
    return (int64_t)jerry_get_number_value(value);    
}

bool psj_jerry_to_bool(const jerry_value_t value)
{
    return jerry_get_boolean_value(value);
}

float psj_jerry_to_float(const jerry_value_t value)
{
    return (float)jerry_get_number_value(value);    
}

uint32_t psj_jerry_get_uint32_property(const jerry_value_t jObject, const jerry_char_t *property)
{
    uint32_t prop_value = 0;
    jerry_value_t value = psj_jerry_get_property(jObject, property);

    if (jerry_value_is_number(value))
    {
        prop_value = jerry_value_as_uint32(value);
    }

    jerry_release_value(value);
    return prop_value;
}

jerry_value_t psj_jerry_get_property(const jerry_value_t jObject, const jerry_char_t *property)
{
    jerry_value_t propertyValue = jerry_create_string(property);
    jerry_value_t value = jerry_get_property(jObject, propertyValue);
    jerry_release_value(propertyValue);

    return value;
}

void psj_jerry_set_property(const jerry_value_t jObject, const jerry_char_t *property, jerry_value_t value)
{
    jerry_value_t propertyValue = jerry_create_string(property);
    jerry_value_t retValue = jerry_set_property(jObject, propertyValue, value);
    jerry_release_value(propertyValue);

    // TODO: Check if retValue returned an error
    jerry_release_value(retValue);
}

void psj_jerry_set_string_property(const jerry_value_t jObject, const jerry_char_t *property, const jerry_char_t *value)
{
    jerry_value_t valueValue;
    
    if (value == NULL)
    {
        valueValue = jerry_create_null();
    }
    else
    {
        valueValue = jerry_create_string(value);
    }

    psj_jerry_set_property(jObject, property, valueValue);
    
    jerry_release_value(valueValue);
}

void psj_jerry_set_uint32_property(const jerry_value_t jObject, const jerry_char_t *property, const uint32_t value)
{
    jerry_value_t valueValue = jerry_create_number(value);
    psj_jerry_set_property(jObject, property, valueValue);
    jerry_release_value(valueValue); 
}

jerry_char_t *psj_jerry_stringify(const jerry_value_t jObject)
{
    jerry_value_t value = jerry_json_stringify(jObject);
    jerry_size_t req_sz = jerry_get_utf8_string_size(value);
    jerry_char_t *str_buf_p = malloc(sizeof(jerry_char_t) * (req_sz+1));

    jerry_string_to_utf8_char_buffer(value, str_buf_p, req_sz);
    str_buf_p[req_sz] = 0;

    jerry_release_value(value);
    return str_buf_p;
}

jerry_char_t *S(const jerry_char_t *txt)
{
    size_t sz = strlen(txt);
    jerry_char_t *sVal = malloc(sizeof(jerry_char_t) * (sz + 1));
    strncpy(sVal, txt, sz);
    return sVal;
}

jerry_char_t *VS(const jerry_char_t *format,va_list args)
{
    va_list szArgs;
    va_copy(szArgs, args);
    size_t sz = vsnprintf(NULL, 0, format, szArgs);
    va_end(szArgs);

    jerry_char_t *sVal = malloc(sizeof(jerry_char_t) * (sz + 1));
    size_t chkSz = vsprintf(sVal, format, args);
    
    assert(sz == chkSz);

    return sVal;
}

jerry_char_t *psj_jerry_vfget_error_message(const PSJErrorCode error_code, va_list args)
{
    jerry_char_t *errMsg;

    switch (error_code)
    {
        case OK:
            return S("OK");

        case UNKNOWN_ERROR:
            return S("An unknown error has occurred");

        case MISSING_ARG:
            return VS("Missing required argument '%s'", args);

        case UNKNOWN_CMD:
            return VS("Unknown command '%s'", args);

        case WRITE_ERROR:
            return VS("Error writing to '%s'", args);

        case READ_ERROR:
            return VS("Error reading from '%s'", args);

        case FILE_NOT_FOUND:
            return VS("File '%s' not found", args);

        case INVALID_ARG:
            return VS("Invalid argument for '%s'. Valid values include %s", args);

        case JAVASCRIPT_ERROR:
            return S("Javascript Error");

        case RUNTIME_ARG_ERROR:
            return VS("Argument error calling '%s'", args);
            
        default:
            return S("Undefined Error");
    }
}

jerry_char_t *psj_jerry_get_error_message(const PSJErrorCode error_code, ...)
{
    va_list args;
    va_start(args, error_code);
    jerry_char_t *msg = psj_jerry_vfget_error_message(error_code, args);
    va_end(args);

    return msg;
}

jerry_value_t psj_jerry_create_error_obj(const PSJErrorCode error_code, ...)
{
    jerry_value_t error_obj = jerry_create_object();
    jerry_char_t *error_msg;

    va_list args;
    va_start(args, error_code);
    error_msg = psj_jerry_vfget_error_message(error_code, args);
    va_end(args);

    psj_jerry_set_uint32_property(error_obj, "error", error_code);
    psj_jerry_set_string_property(error_obj, "message", error_msg);

    free(error_msg);
    return error_obj;
}

jerry_value_t psj_jerry_exception_to_error_obj(jerry_value_t exception)
{
    assert(jerry_value_is_error(exception));

    jerry_value_t error_obj = jerry_create_object();
    jerry_value_t error_val = jerry_get_value_from_error(exception, false);
    jerry_char_t *error_msg = psj_jerry_get_string_property(error_val, "message");

    psj_jerry_set_uint32_property(error_obj, "error", JAVASCRIPT_ERROR);
    psj_jerry_set_string_property(error_obj, "message", error_msg);

    free(error_msg);
    jerry_release_value(error_val);

    return error_obj;
}

jerry_char_t *psj_jerry_exception_to_string(jerry_value_t exception)
{
    jerry_value_t error_obj = psj_jerry_exception_to_error_obj(exception);
    jerry_char_t *str = psj_jerry_stringify(error_obj);

    jerry_release_value(error_obj);

    return str;
}
