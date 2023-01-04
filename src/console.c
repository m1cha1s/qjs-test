#include <quickjs.h>

#include "console.h"
#include "utils.h"

static JSValue js_console_log(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    const char *str;

    for (int i = 0; i < argc; i++)
    {
        if (i != 0)
            PRINT(" ");
        str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;
        PRINT(str);
        JS_FreeCString(ctx, str);
    }
    printf("\n");
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry js_fs_funcs[] = {
    JS_CFUNC_DEF("log", 1, js_console_log),
};

static int js_console_init(JSContext *ctx, JSModuleDef *js_console)
{
    return JS_SetModuleExportList(ctx, js_console, js_fs_funcs, COUNTOF(js_fs_funcs));
}

JSModuleDef *js_init_console(JSContext *ctx)
{
    JSModuleDef *js_console = JS_NewCModule(ctx, "console", js_console_init);

    if (!js_console)
        return NULL;

    JS_AddModuleExportList(ctx, js_console, js_fs_funcs, COUNTOF(js_fs_funcs));
    return js_console;
}