bash make.sh

if [ $# == 0 ] ; then
    build/test/pikascript_test
fi

if [ $# == 1 ] ; then
	filter=$1
    build/test/pikascript_test --gtest_filter=$filter
fi
