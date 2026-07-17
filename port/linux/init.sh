ROOT=$PWD
reset
cp config/pika_config_default.h config/pika_config.h

# Linux core tests do not require optional board or LVGL submodules.
git config --unset-all core.filemode
git config --add core.filemode false
chmod +x *.sh 
sh update-compiler.sh
chmod +x package/pikascript/rust-msc-latest-linux
cp package/pikascript/rust-msc-latest-linux /bin 
find package/pikascript/pikascript-lib/pika_lvgl -type f -delete
find package/pikascript -maxdepth 1 -name 'pika_lvgl.pyi' -type f -delete
find package/pikascript/pikascript-api -maxdepth 1 \
    \( -name '__pikaBinding.c' -o -name 'pika_lvgl*.h' \) -type f -delete
cd package/pikascript/pikascript-core
# git checkout master
cd $ROOT
rm build -rf
mkdir build
./package/pikascript/rust-msc-latest-linux -c package/pikascript/cjson_test.py
sh api-make.sh
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug -DPIKA_CONFIG_ENABLE=ON ..

cd $ROOT
rm Release -rf
mkdir Release
cd Release 
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DPIKA_CONFIG_ENABLE=ON ..

cd $ROOT
