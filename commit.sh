ROOT=$PWD
cd src/package/pikascript-core
git add *
git commit
cd $ROOT
git add *
git add .vscode/launch.json
git add .gitignore
git commit

