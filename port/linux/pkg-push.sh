pkg=$1
cp package/pikascript/pikascript-lib/$pkg ../../package/ -r
cp package/pikascript/$pkg.py ../../package/$pkg/ 
git add ../../package/$pkg
