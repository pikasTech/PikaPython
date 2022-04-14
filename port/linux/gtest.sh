# backup
cp config/pika_config.h config/pika_config_backup.h

cp config/pika_config_default.h config/pika_config.h && sh _gtest_once.sh && \
cp config/pika_config_builtin_list.h config/pika_config.h && sh _gtest_once.sh && \
cp config/pika_config_benchmark.h config/pika_config.h && sh _gtest_once.sh 

# restore backup
mv config/pika_config_backup.h config/pika_config.h