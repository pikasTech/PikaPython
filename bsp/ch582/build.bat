@echo off

if exist build (
echo Found build/ 
cd build
cmake -GNinja ..
ninja
) else (
echo Not Found build/
mkdir build
cd build
cmake -GNinja ..
ninja
)
