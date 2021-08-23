ROOT=$PWD
cd src/package/pikascript/pikascript-core
git add *
git commit
cd $ROOT
git add *
git add .vscode/launch.json
git add .vscode/c_cpp_properties.json
git add .gitignore
git add src/package/pikascript/pikascript-api/path -f
git commit

