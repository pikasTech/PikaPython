if [ $# == 0 ] ; then
	valgrind -s --track-origins=yes --leak-check=full --show-leak-kinds=all  build/test/pikascript_test 
fi

if [ $# == 1 ] ; then
	filter=$1
	valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all  build/test/pikascript_test --gtest_filter=$filter
fi
