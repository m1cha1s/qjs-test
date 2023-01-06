#include <quickjs.h>
#include <string.h>

#include "fs.h"
#include "utils.h"

static JSClassID js_file_class_id;

typedef struct JSFile
{
    FILE_T f;
    bool close_in_finalizer;
} JSFile;

static JSValue js_new_file(JSContext *ctx, FILE_T f, bool close_in_finalizer)
{
    JSFile *s;
    JSValue obj = JS_NewObjectClass(ctx, js_file_class_id);
    if (JS_IsException(obj))
        return obj;
    s = js_malloc(ctx, sizeof(*s));
    if (!s)
    {
        JS_FreeValue(ctx, obj);
        return JS_EXCEPTION;
    }
    s->close_in_finalizer = close_in_finalizer;
    s->f = f;
    JS_SetOpaque(obj, s);
    return obj;
}

static JSValue js_file_constructor(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    const char *filename, *mode = NULL;
    FILE_T f;
    int err;

    filename = JS_ToCString(ctx, argv[0]);
    if (!filename)
        goto FAIL;

    mode = JS_ToCString(ctx, argv[1]);
    if (!mode)
        goto FAIL;

    if (mode[strspn(mode, "rwa+b")] != '\0')
    {
        JS_ThrowTypeError(ctx, "invalid file mode");
        goto FAIL;
    }

    f = FILE_OPEN(filename, mode);

    JS_FreeCString(ctx, filename);
    JS_FreeCString(ctx, mode);
    if (!f)
        return JS_NULL;

    return js_new_file(ctx, f, true);

FAIL:
    JS_FreeCString(ctx, filename);
    JS_FreeCString(ctx, mode);
    return JS_EXCEPTION;
}

static void js_file_finalizer(JSRuntime *rt, JSValue val)
{
    JSFile *s = JS_GetOpaque(val, js_file_class_id);
    if (s)
    {
        if (s->f && s->close_in_finalizer)
        {
            FILE_CLOSE(s->f);
        }
        js_free_rt(rt, s);
    }
}

static FILE_T js_file_get(JSContext *ctx, JSValueConst obj)
{
    JSFile *s = JS_GetOpaque2(ctx, obj, js_file_class_id);
    if (!s)
        return NULL;
    if (!FILE_OK(s->f))
    {
        JS_ThrowTypeError(ctx, "invalid file handle");
        return NULL;
    }
    return s->f;
}

static JSValue js_file_read_string(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    switch (argc)
    {
    case 0:
    {
        FILE_T f = js_file_get(ctx, this_val);
        if (!FILE_OK(f))
            goto FAIL;
    }

    default:
        goto FAIL;
    }

FAIL:
    return JS_NULL;
}

static JSClassDef js_file_class = {
    "File",
    .finalizer = js_file_finalizer,
};

static const JSCFunctionListEntry js_fs_funcs[] = {
    // JS_CFUNC_DEF("log", 1, js_console_log),
};

static const JSCFunctionListEntry js_fs_proto_funcs[] = {

};

static int js_fs_init(JSContext *ctx, JSModuleDef *js_fs)
{
    JS_NewClassID(&js_file_class_id);
    JS_NewClass(JS_GetRuntime(ctx), js_file_class_id, &js_file_class);

    JSValue proto = JS_NewObject(ctx);

    JS_SetPropertyFunctionList(ctx, proto, js_fs_proto_funcs, COUNTOF(js_fs_funcs));
    JS_SetClassProto(ctx, js_file_class_id, proto);

    JSValue file_class = JS_NewCFunction2(ctx, js_file_constructor, "File", 2, JS_CFUNC_constructor, 0);
    JS_SetConstructor(ctx, file_class, proto);

    JS_SetModuleExport(ctx, js_fs, "File", file_class);
    JS_SetModuleExportList(ctx, js_fs, js_fs_funcs, COUNTOF(js_fs_funcs));

    return 0;
}

JSModuleDef *js_init_fs(JSContext *ctx)
{
    JSModuleDef *js_fs = JS_NewCModule(ctx, "fs", js_fs_init);

    if (!js_fs)
        return NULL;

    JS_AddModuleExport(ctx, js_fs, "File");
    JS_AddModuleExportList(ctx, js_fs, js_fs_funcs, COUNTOF(js_fs_funcs));
    return js_fs;
}