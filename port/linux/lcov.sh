lcov --capture --directory build/package/pikascript/CMakeFiles/pikascript-core.dir/pikascript-core --output-file  testHtml.info --test-name testHtml
genhtml -o result testHtml.info

