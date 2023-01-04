cmake_minimum_required(VERSION 3.16)

project(qjs)

add_library(qjs
    quickjs/libbf.c
    quickjs/cutils.c
    quickjs/libregexp.c
    quickjs/libunicode.c
    quickjs/quickjs.c
)

target_compile_definitions(qjs PRIVATE CONFIG_VERSION="2021-03-27")
target_include_directories(qjs PUBLIC quickjs/)
target_link_libraries(qjs PRIVATE m)

project(qjs-test)

add_executable(qjs-test
    main.c
    console.c
)

target_link_libraries(qjs-test PRIVATE qjs)
