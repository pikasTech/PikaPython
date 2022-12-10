clear
rm /mnt/d/pikascript_bl602.bin
cd pikascript && wine rust-msc-latest-win10.exe
cd -
make -j
cp build_out/pikascript_bl602.bin /mnt/d