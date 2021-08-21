sh clean.sh
git submodule init
git submodule update
mkdir build
cd build
cmake ..
cd ..
sh make.sh
