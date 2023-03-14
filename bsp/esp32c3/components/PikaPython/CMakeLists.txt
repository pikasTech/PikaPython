file(GLOB_RECURSE SOURCES *.c)
idf_component_register(
                SRCS ${SOURCES}
                INCLUDE_DIRS 
                "pikascript-api" 
                "pikascript-core"
                "pikascript-lib/PikaStdLib"
                "pikascript-lib/PikaStdDevice"
                "pikascript-lib/ESP32"
                "../../main"
                WHOLE_ARCHIVE
                REQUIRES driver
                )