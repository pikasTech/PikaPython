cp config/pika_config_void.h config/pika_config.h
sh api-make.sh
sh only_make.sh
cp ./build/boot/demo06-pikamain/pikascript_demo06-pikamain package/pikascript/pika
cp config/pika_config_default.h config/pika_config.h
