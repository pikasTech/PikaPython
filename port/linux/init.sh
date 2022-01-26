ROOT=$PWD
# git submodule init
# git submodule update
git config --add core.filemode false
chmod +x *.sh 
sh update-compiler.sh
chmod +x package/pikascript/rust-msc-latest-linux
rm package/pikascript/pikascript-api -rf
mkdir package/pikascript/pikascript-api
touch package/pikascript/pikascript-api/path
cd package/pikascript/pikascript-core
# git checkout master
cd $ROOT
rm build -rf
mkdir build
sh api-make.sh
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

cd $ROOT
rm Release -rf
mkdir Release
cd Release 
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j

cd $ROOT
sh make.sh
