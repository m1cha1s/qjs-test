#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <quickjs.h>

#include "console.h"
#include "fs.h"
#include "utils.h"

static void printJSException(JSContext *ctx)
{
    JSValue exception = JS_GetException(ctx);
    const char *exceptionString = JS_ToCString(ctx, exception);
    PRINT(exceptionString);
    PRINT("\n");

    if (JS_IsError(ctx, exception))
    {
        JSValue stackValue = JS_GetPropertyStr(ctx, exception, "stack");
        if (!JS_IsUndefined(stackValue))
        {
            const char *stackString = JS_ToCString(ctx, stackValue);
            PRINT(stackString);
            PRINT("\n");

            JS_FreeCString(ctx, stackString);
        }
        JS_FreeValue(ctx, stackValue);
    }

    JS_FreeCString(ctx, exceptionString);
    JS_FreeValue(ctx, exception);
}

static char *loadFile(JSContext *ctx, const char *filename, size_t *file_size)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        goto ERR2;

    fseek(file, 0L, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    char *file_contents = (char *)js_malloc(ctx, *file_size);
    if (!file_contents)
        goto ERR;

    fread(file_contents, sizeof(char), *file_size, file);

    fclose(file);
    return file_contents;
ERR:
    fclose(file);
ERR2:
    return NULL;
}

static JSModuleDef *js_module_loader(JSContext *ctx, const char *module_name, void *opaque)
{
    JSModuleDef *m;

    size_t fsize;
    char *buf = loadFile(ctx, module_name, &fsize);
    if (!buf)
    {
        JS_ThrowReferenceError(ctx, "could not load module filename '%s'", module_name);
        return NULL;
    }

    JSValue func_val = JS_Eval(ctx, buf, fsize, module_name, JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY);
    js_free(ctx, buf);

    if (JS_IsException(func_val))
        return NULL;

    m = JS_VALUE_GET_PTR(func_val);
    JS_FreeValue(ctx, func_val);

    return m;
}

static JSContext *JS_NewCustomContext(JSRuntime *rt)
{
    JSContext *ctx = JS_NewContext(rt);
    js_init_console(ctx);
    JS_SetModuleLoaderFunc(rt, NULL, js_module_loader, NULL);

    return ctx;
}

int main()
{
    JSRuntime *rt = JS_NewRuntime();
    JSContext *ctx = JS_NewCustomContext(rt);

    JSValue glob = JS_GetGlobalObject(ctx);

    size_t file_size;
    char *file_contents = loadFile(ctx, "../test.js", &file_size);
    if (!file_contents)
        goto EXIT;

    JSValue val = JS_Eval(ctx, file_contents, file_size, "main", JS_EVAL_TYPE_MODULE);
    js_free(ctx, file_contents);
    if (JS_IsException(val))
    {
        printJSException(ctx);
    }
    JS_FreeValue(ctx, val);

EXIT:
    JS_FreeValue(ctx, glob);
    JS_RunGC(rt);

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    return 0;
}