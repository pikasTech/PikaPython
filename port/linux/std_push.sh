pkg=$1
cp package/pikascript/pikascript-lib/PikaStdLib ../../package/ -r
cp package/pikascript/$pkg.pyi ../../package/PikaStdLib/ 

cp package/pikascript/pikascript-lib/PikaStdLib ../../tools/pikaByteCodeGen/pikascript/pikascript-lib -r
cp package/pikascript/$pkg.pyi ../../tools/pikaByteCodeGen/pikascript

git add package/pikascript/pikascript-lib/PikaStdLib
git add package/pikascript/$pkg.pyi
git add ../../package/PikaStdLib
