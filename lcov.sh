lcov --capture --directory port/linux/build/package/pikascript/CMakeFiles/pikascript-core.dir/pikascript-core --output-file  testHtml.info --test-name testHtml
genhtml -o result testHtml.info

# replace port/linux/package/pikascript/pikascript-core/ to src/ for testHtml.info
sed -i 's/port\/linux\/package\/pikascript\/pikascript-core/src/g' testHtml.info

