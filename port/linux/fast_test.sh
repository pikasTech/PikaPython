
ROOT=$PWD
rm $(find build -name *.gcda) -f
cd build && rm ./test/pikascript_test -f &&  ninja -j0 
cd $ROOT
if [ $# == 0 ] ; then
    build/test/pikascript_test
fi

if [ $# == 1 ] ; then
	filter=$1
    build/test/pikascript_test --gtest_filter=$filter
fi
