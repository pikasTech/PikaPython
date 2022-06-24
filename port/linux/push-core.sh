cp package/pikascript/pikascript-core/* ../../src -r
cp package/pikascript/pikascript-core/* ../../tools/pikaByteCodeGen/pikascript/pikascript-core -r
cp package/pikascript/PikaObj.pyi ../../src
git add test/python/*.py
sh std_push.sh PikaStdLib
sh std_push.sh PikaStdData
sh std_push.sh PikaDebug
sh std_push.sh PikaStdTask
