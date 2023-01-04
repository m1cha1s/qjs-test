#ifndef CONSOLE_H
#define CONSOLE_H

#include <quickjs.h>

#ifndef PRINT
#define PRINT(x) printf("%s", x)
#endif

JSModuleDef *js_init_console(JSContext *ctx);

#endif