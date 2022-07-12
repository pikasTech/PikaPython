if [ $# != 1 ] ; then
echo "USAGE: $0 [pkg name]"
echo " e.g.: $0 ctypes"
exit 1;
fi
pkg=$1
cp package/pikascript/pikascript-lib/$pkg ../../package/ -r
cp package/pikascript/$pkg.pyi ../../package/$pkg/
cp package/pikascript/$pkg.py ../../package/$pkg/

git add package/pikascript/pikascript-lib/$pkg
git add package/pikascript/$pkg.pyi 
git add package/pikascript/$pkg.py 

git add ../../package/$pkg
