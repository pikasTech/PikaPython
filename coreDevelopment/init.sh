ROOT=$PWD
# git submodule init
# git submodule update
sh update-compiler.sh
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
sh make.sh
