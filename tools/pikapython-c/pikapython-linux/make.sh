# create build dir if not exists
if [ ! -d "build" ]; then
  mkdir build
  cd build
    cmake ..
    cd ..
fi

# before build
cd pikapython
  wine rust-msc-latest-win10.exe
cd ..

# build
cd build
  make
cd ..

# run
./build/pikapython
