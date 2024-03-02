#include "callback_engine.h"
#include "jerryscript.h"
#include "jerry_helper.h"

#include <stdlib.h>
#include <string.h>

#define TEST_INIT() int test_result = 0; printf("Running '%s'\n", __func__);
#define TEST_FAIL() { test_result = 1; goto cleanup; }
#define TEST_FINISH() return test_result;

#define ASSERT_NOT_ERROR(value) { printf("Asserting '" #value "' not error\n"); if (jerry_value_is_error(value)) { jerry_char_t *str = psj_jerry_exception_to_string(value); printf("  FAILED\n  Error: %s\n", str); free(str); TEST_FAIL(); } }
#define ASSERT_INT(test, expected) { printf("Asserting '" #test "'\n  expected: %d\n", expected); int actual = test; printf("  actual: %d\n", actual); if (expected != actual) { printf("  FAILED\n"); TEST_FAIL(); } }
#define ASSERT_BOOL(test, expected) { printf("Asserting '" #test "'\n  expected: %s\n", expected ? "true" : "false"); bool actual = test; printf("  actual: %s\n", actual ? "true" : "false"); if (expected != actual) { printf("  FAILED\n"); TEST_FAIL(); } }
#define ASSERT_STR(test, expected) { \
    printf("Asserting '" #test "'\n  expected: %s\n", expected); \
    jerry_char_t *actual = test; \
    printf("  actual: %s\n", actual); \
    if (strcmp(expected, actual) != 0) { \
        free(actual); \
        printf("  FAILED\n"); \
        TEST_FAIL(); \
    } else { \
        free(actual); \
    } \
}

int verify_hash_TEST()
{
    TEST_INIT();

    ASSERT_INT(hash(0x12, 0xFDCBA9), 0x12FDCBA9);

cleanup:
    TEST_FINISH();
}

int register_callback_TEST()
{
    TEST_INIT();

    jerry_init(JERRY_INIT_EMPTY);

    const jerry_char_t fnScript[] = "(x) => { return `x = ${x}`; }";
    jerry_value_t result = 0;

    jerry_value_t callbackFunction = jerry_eval(fnScript, strlen(fnScript), JERRY_PARSE_NO_OPTS);
    ASSERT_BOOL(jerry_value_is_function(callbackFunction), true);

    register_callback(CALLBACK_GENERIC, 0, callbackFunction);
    result = invoke_callback(CALLBACK_GENERIC, 0);

    ASSERT_NOT_ERROR(result);
    ASSERT_INT(jerry_value_get_type(result), JERRY_TYPE_STRING);
    ASSERT_STR(psj_jerry_to_string(result), "x = 0");
    
    remove_callback(CALLBACK_GENERIC, 0);

cleanup:
    if (result != 0) jerry_release_value(result);

    jerry_cleanup();
    TEST_FINISH();
}

int main() 
{
    return verify_hash_TEST() 
        | register_callback_TEST();
}