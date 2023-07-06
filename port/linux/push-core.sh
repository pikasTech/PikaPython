#!/bin/sh
FLAG_OK="\033[32m[ OK ]\033[0m"
FLAG_INFO="\033[32m[Info]\033[0m"
FLAG_NOTE="\033[35m[Note]\033[0m"

if [ -d "../../../pikalab" ]; then
    sh lab-push.sh
fi

cp package/pikascript/pikascript-core/* ../../src -r
cp package/pikascript/PikaObj.pyi ../../src
git add $(find test/python -name '*.py')
git add $(find test -name '*.cpp')
sh std_push.sh PikaStdLib
sh std_push.sh PikaStdData
sh std_push.sh PikaDebug
sh std_push.sh PikaStdTask
sh std_push.sh builtins
echo "$FLAG_OK Push \033[32mpikascript-core\033[0m to ../../src successfully!"
echo "$FLAG_OK Push \033[32mPikaSdLib\033[0m to ../../package/PikaStdLib successfully!"
echo "$FLAG_NOTE Now, you can run 'git commit -a' to commit changes."
echo "$FLAG_NOTE Then, you can run 'git push' to push to github/gitee."
