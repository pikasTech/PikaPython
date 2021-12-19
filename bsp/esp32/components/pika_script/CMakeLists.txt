file(GLOB_RECURSE srcs *.c)

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS pikascript-api pikascript-core
)
