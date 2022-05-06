ROOT=$PWD
cd ../../tools/pikaCompiler
cargo clean
sh build.sh
cd $ROOT
cp ../../tools/pikaCompiler/rust-msc-latest-linux package/pikascript/rust-msc-latest-linux
