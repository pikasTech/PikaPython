if [ -d "../../../pikalab" ]; then
    sh lab-pull.sh
fi
rm -rf package/pikascript/pikascript-core
cp -r ../../src package/pikascript/pikascript-core
