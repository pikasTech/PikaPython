rm build-win -rf && mkdir build-win && cd build-win && cmake --toolchain ../CrossCompile.cmake .. && make -j
