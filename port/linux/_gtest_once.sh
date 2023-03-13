# sh update-compiler.sh
rm build/test/pikascript_test -f
rm $(find build -name *.gcda)
sh make.sh
build/test/pikascript_test