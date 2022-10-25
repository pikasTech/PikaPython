ROOT=$PWD
rm $(find build -name *.gcda) -f
cd build && rm ./test/pikascript_test -f &&  ninja -j0 
cd $ROOT
build/test/pikascript_test
