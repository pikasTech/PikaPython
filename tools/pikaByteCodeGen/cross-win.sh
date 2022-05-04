rm build-win -rf && mkdir build-win && cd build-win && cmake -DCMAKE_TOOLCHAIN_FILE=../CrossCompile.cmake .. && make -j
