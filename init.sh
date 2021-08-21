ROOT=$PWD
git submodule init
git submodule update
cd src/package/pikascript-core
git checkout master
cd $ROOT
rm build -rf
mkdir build
cd build
cmake ..
cd $ROOT
sh make.sh
