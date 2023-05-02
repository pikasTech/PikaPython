# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")
# add macro
idf_build_set_property(
    COMPILE_DEFINITIONS "-DPIKA_CONFIG_ENABLE" APPEND)
