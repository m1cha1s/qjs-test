#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <quickjs.h>
#include <stdbool.h>

#ifndef FILE_T
#define FILE_T FILE *
#endif

#ifndef FILE_CLOSE
#define FILE_CLOSE(file) fclose(file)
#endif

#ifndef FILE_OPEN
#define FILE_OPEN(filename, mode) fopen(filename, mode)
#endif

#ifndef FILE_SEEK
#define FILE_SEEK(file, offset, origin) fseek(file, offset, origin)
#endif

#ifndef FILE_TELL
#define FILE_TELL(file) ftell(file)
#endif

#ifndef FILE_REWIND
#define FILE_REWIND(file) rewind(file)
#endif

#ifndef FILE_read
#define FILE_read(buff, size, count, file) fread(buff, size, count, file)
#endif

#ifndef FILE_END
#define FILE_END SEEK_END
#endif

#ifndef FILE_OK
#define FILE_OK(file) (file)
#endif

JSModuleDef *
js_init_fs(JSContext *ctx);

#endif