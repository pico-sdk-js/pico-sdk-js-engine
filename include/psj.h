#include "flash.h"
#include "io.h"
#include "jerry_helper.h"
#include "jerryscript-port-psj.h"
#include "os.h"
#include "repl.h"
#include "core1.h"
#include "interrupt_state.h"
#include "modules.h"

#ifndef TARGET_NAME
#define TARGET_NAME "UNKNOWN"
#endif

#ifndef TARGET_VERSION
#define TARGET_VERSION "0.0.0"
#endif

