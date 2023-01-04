#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <quickjs.h>

static JSValue print(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    const char *str;

    for (int i = 0; i < argc; i++)
    {
        if (i != 0)
            printf(" ");
        str = JS_ToCString(ctx, argv[i]);
        if (!str)
            return JS_EXCEPTION;
        printf("%s", str);
        JS_FreeCString(ctx, str);
    }
    printf("\n");
    return JS_UNDEFINED;
}

static const JSCFunctionListEntry std_funcs[] = {
    JS_CFUNC_DEF("print", 1, print),
};

static char *loadFile(const char *filename, size_t *file_size)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        goto ERR2;

    fseek(file, 0L, SEEK_END);
    *file_size = ftell(file);
    rewind(file);

    char *file_contents = (char *)malloc(*file_size);
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

int main()
{
    JSRuntime *rt = JS_NewRuntime();
    JSContext *ctx = JS_NewContext(rt);

    JSValue glob = JS_GetGlobalObject(ctx);
    JS_SetPropertyFunctionList(ctx, glob, std_funcs, 1);

    size_t file_size;
    char *file_contents = loadFile("../test.js", &file_size);
    if (!file_contents)
        goto EXIT;

    JSValue val = JS_Eval(ctx, file_contents, file_size, "main", JS_EVAL_TYPE_GLOBAL);
    free(file_contents);
    if (JS_IsException(val))
    {
        JSValue exc = JS_GetException(ctx);
        printf("[EXCEPTION] %s\n", JS_ToCString(ctx, exc));
        JS_FreeValue(ctx, exc);
    }
    JS_FreeValue(ctx, val);

EXIT:
    JS_FreeValue(ctx, glob);
    JS_RunGC(rt);

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    return 0;
}