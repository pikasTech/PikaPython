set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-char-subscripts -Wno-maybe-uninitialized")

file(GLOB_RECURSE SOURCES *.c)
idf_component_register(
                SRCS ${SOURCES}
                INCLUDE_DIRS 
                "pikascript-api" 
                "pikascript-core"
                "pikascript-lib/PikaStdLib"
                "pikascript-lib/PikaStdDevice"
                "pikascript-lib/ESP32"
                "pikascript-lib/pika_libc"
                "../../main"
                WHOLE_ARCHIVE
                REQUIRES
                driver
                fatfs
                esp_wifi
                nvs_flash
                app_update
                esp_http_client
                esp_https_ota
                )