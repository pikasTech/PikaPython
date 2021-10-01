ROOT=$PWD
cd ../pikaCompiler
cargo build
cd $ROOT
cp ../pikaCompilertarget/debug/rust-msc package/pikascript/rust-msc-latest-linux
cp ../pikaCompilertarget/debug/rust-msc.exe package/pikascript/rust-msc-latest-win10.exe
