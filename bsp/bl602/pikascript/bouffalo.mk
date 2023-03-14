COMPONENT_ADD_INCLUDEDIRS += pikascript-core \
							 pikascript-api \
							 pikascript-lib/PikaStdDevice \
							 pikascript-lib/BLIOT \

COMPONENT_SRCS := 

CFLAGS += -DPIKASCRIPT -DPIKA_CONFIG_ENABLE -DCONFIG_SYS_VFS_ENABLE=1 \
          -DLFS_YES_TRACE

COMPONENT_OBJS := $(patsubst %.c,%.o, $(COMPONENT_SRCS))

COMPONENT_SRCDIRS := pikascript-core \
					 pikascript-api \
					 pikascript-lib/BLIOT \
					 pikascript-lib/PikaStdDevice \
					 pikascript-lib/PikaStdLib

COMPONENT_ADD_LDFLAGS_HEAD = -Wl,--whole-archive
COMPONENT_ADD_LDFLAGS_TAIL = -Wl,--no-whole-archive
