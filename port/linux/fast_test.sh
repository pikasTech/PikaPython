ROOT=$PWD
rm $(find build -name *.gcda) -f
cd build && rm ./test/pikascript_test -f &&  ninja -j16 
cd $ROOT
build/test/pikascript_test
