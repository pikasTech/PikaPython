git submodule init
git submodule update
sh clean.sh
mkdir build
cd build
cmake ..
cd ..
sh make.sh
