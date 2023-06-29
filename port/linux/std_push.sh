pkg=$1
cp package/pikascript/pikascript-lib/PikaStdLib ../../package/ -r
cp package/pikascript/$pkg.pyi ../../package/PikaStdLib/ 

git add package/pikascript/pikascript-lib/PikaStdLib
git add package/pikascript/$pkg.pyi
git add ../../package/PikaStdLib

# check if test/python/$pkg folder is exit
if [ -d "test/python/$pkg" ] ; then
echo "$FLAG_INFO python test files added:"
rm ../../examples/$pkg -rf
mkdir ../../examples/$pkg
cp test/python/$pkg/*.py ../../examples/$pkg -r
git add ../../test/python/$pkg
git add ../../examples/$pkg
ls ../../examples/$pkg
fi
