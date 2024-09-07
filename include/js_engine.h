#ifndef __JS_ENGINE_H
#define __JS_ENGINE_H

#include "jerryscript.h"

#define JSENGINE_STATE_STOPPED          (0)
#define JSENGINE_STATE_RUNNING          (1)
#define JSENGINE_STATE_REQ_STOPPING     (2)
#define JSENGINE_STATE_AWAIT_STOPPING   (3)

int8_t jsengine_get_state();
void jsengine_set_state(int8_t state);

char* psj_get_current_resource();
void psj_set_next_resource(const char* nextResource);
void psj_run_js_loop();

#endif // __JS_ENGINE_H
