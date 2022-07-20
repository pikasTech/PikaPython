FLAG_OK="\033[32m[ OK ]\033[0m"
FLAG_INFO="\033[32m[Info]\033[0m"
FLAG_NOTE="\033[35m[Note]\033[0m"
FLAG_ERROR="\033[31m[Error]\033[0m"

if [ $# != 1 ] ; then
echo "USAGE: $0 [pkg name]"
echo " e.g.: $0 ctypes"
exit 1;
fi

pkg=$1

# Check if the package exists
if [ ! -d package/pikascript/pikascript-lib/$pkg ] ; then
echo -e "$FLAG_ERROR Package $pkg does not exist"
exit 1;
fi

cp package/pikascript/pikascript-lib/$pkg ../../package/ -r
git add package/pikascript/pikascript-lib/$pkg

# check if $pkg.pyi is exit
if [ -f "package/pikascript/$pkg.pyi" ] ; then
echo -e "$FLAG_INFO python interface files added"
cp package/pikascript/$pkg.pyi ../../package/$pkg/
git add package/pikascript/$pkg.pyi 
echo "$pkg.pyi"
fi

# check if $pkg.py is exit
if [ -f "package/pikascript/$pkg.py" ] ; then
echo -e "$FLAG_INFO python files added"
cp package/pikascript/$pkg.py ../../package/$pkg/
git add package/pikascript/$pkg.py 
echo "$pkg.py"
fi

# check if test/python/$pkg folder is exit
if [ -d "test/python/$pkg" ] ; then
echo -e "$FLAG_INFO python test files added:"
cp test/python/$pkg/*.py ../../examples/$pkg -r
git add ../../examples/$pkg
ls ../../examples/$pkg
fi

git add ../../package/$pkg
echo -e "$FLAG_INFO lib files added:"
# list files name in package/pikascript/pikascript-lib/$pkg
ls  package/pikascript/pikascript-lib/$pkg
echo -e "$FLAG_OK Push \033[32m$pkg\033[0m to ../../package/$pkg successfully!"
echo -e "$FLAG_NOTE Now, you can run 'git commit -a' to commit changes."
echo -e "$FLAG_NOTE Then, you can run 'git push' to push to github/gitee."
