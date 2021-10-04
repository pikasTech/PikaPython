ROOT=$PWD
cd ../tools/pikaCompiler
cargo build
cd $ROOT
cp ../tools/pikaCompilertarget/debug/rust-msc package/pikascript/rust-msc-latest-linux
cp ../tools/pikaCompilertarget/debug/rust-msc.exe package/pikascript/rust-msc-latest-win10.exe
