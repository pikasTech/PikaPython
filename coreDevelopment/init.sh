ROOT=$PWD
git submodule init
git submodule update
sh update-compiler.sh
rm coreDevelopment/package/pikascript/pikascript-api -rf
mkdir coreDevelopment/package/pikascript/pikascript-api
touch coreDevelopment/package/pikascript/pikascript-api/path
cd coreDevelopment/package/pikascript/pikascript-core
git checkout master
cd $ROOT
rm build -rf
mkdir build
sh api-make.sh
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cd $ROOT
sh make.sh
