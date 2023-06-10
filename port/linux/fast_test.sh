bash make.sh

find build -name "*.gcda" -type f -delete

if [ $# == 0 ] ; then
    build/test/pikascript_test
fi

if [ $# == 1 ] ; then
	filter=$1
    build/test/pikascript_test --gtest_filter=$filter
fi
