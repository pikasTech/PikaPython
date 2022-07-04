if [ $# != 1 ] ; then
echo "USAGE: $0 [pkg name]"
echo " e.g.: $0 ctypes"
exit 1;
fi
pkg=$1
cp package/pikascript/pikascript-lib/$pkg ../../package/ -r
cp package/pikascript/$pkg.pyi ../../package/$pkg/ -f
cp package/pikascript/$pkg.py ../../package/$pkg/ -f

git add package/pikascript/pikascript-lib/$pkg
git add package/pikascript/$pkg.pyi -f
git add package/pikascript/$pkg.py -f

git add ../../package/$pkg
