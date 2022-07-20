FLAG_OK="\033[32m[ OK ]\033[0m"
FLAG_INFO="\033[32m[Info]\033[0m"
FLAG_NOTE="\033[35m[Note]\033[0m"

if [ $# != 1 ] ; then
echo "USAGE: $0 [pkg name]"
echo " e.g.: $0 ctypes"
exit 1;
fi
pkg=$1
cp package/pikascript/pikascript-lib/$pkg ../../package/ -r
git add package/pikascript/pikascript-lib/$pkg

echo -e "$FLAG_INFO python files added"
# check if $pkg.pyi is exit
if [ -f "package/pikascript/$pkg.pyi" ] ; then
cp package/pikascript/$pkg.pyi ../../package/$pkg/
git add package/pikascript/$pkg.pyi 
echo "$pkg.pyi"
fi

# check if $pkg.py is exit
if [ -f "package/pikascript/$pkg.py" ] ; then
cp package/pikascript/$pkg.py ../../package/$pkg/
git add package/pikascript/$pkg.py 
echo "$pkg.py"
fi

git add ../../package/$pkg
echo -e "$FLAG_INFO C files added:"
# list files name in package/pikascript/pikascript-lib/$pkg
ls  package/pikascript/pikascript-lib/$pkg
echo -e "$FLAG_OK Push $pkg to ../../package successfully!"
echo -e "$FLAG_NOTE Now, you can run 'git commit -a' to commit changes"
echo -e "$FLAG_NOTE Then, you can run 'git push' to push to github/gitee."
