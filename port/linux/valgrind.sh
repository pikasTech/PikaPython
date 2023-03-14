sh only_make.sh
if [ $# == 0 ] ; then
	valgrind -s --track-origins=yes --leak-check=full --show-leak-kinds=all --exit-on-first-error=yes --error-exitcode=1 --num-callers=50 build/test/pikascript_test 
fi

if [ $# == 1 ] ; then
	filter=$1
	valgrind -s --track-origins=yes --leak-check=full --show-leak-kinds=all  --exit-on-first-error=yes --error-exitcode=1 --num-callers=50 build/test/pikascript_test --gtest_filter=$filter 
fi
