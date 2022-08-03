cp config/pika_config_void.h config/pika_config.h
sh only_make.sh
cd package/pikascript && \
./pika $1
