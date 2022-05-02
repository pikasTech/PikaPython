sh init.sh
cp config/pika_config_default.h config/pika_config.h && sh _gtest_once.sh && \
cp config/pika_config_optimize_speed.h config/pika_config.h && sh _gtest_once.sh && \
cp config/pika_config_syntax_level_maximal.h config/pika_config.h && sh _gtest_once.sh 
