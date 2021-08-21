git submodule init
git submodule update
rm build -rf
mkdir build
cd build
cmake ..
cd ..
sh make.sh
