ROOT=$PWD
cd pikaCompiler
cargo build
cd $ROOT
cp pikaCompiler/target/debug/rust-msc coreDevelopment/package/pikascript/rust-msc-latest-linux
cp pikaCompiler/target/debug/rust-msc.exe coreDevelopment/package/pikascript/rust-msc-latest-win10.exe
