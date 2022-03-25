pkg=$1
cp package/pikascript/pikascript-lib/PikaStdLib ../../package/ -r
cp package/pikascript/$pkg.py ../../package/PikaStdLib/ 
git add package/pikascript/pikascript-lib/PikaStdLib
git add package/pikascript/$pkg.py
git add ../../package/PikaStdLib
