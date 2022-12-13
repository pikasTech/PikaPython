ROOT=$PWD
rm -rf libpikabinder
mkdir libpikabinder
cargo build --release
cbindgen --config cbindgen.toml --crate rust-msc --output libpikabinder/libpikabinder.h
cp target/release/libpikabinder.a libpikabinder

cd ../pikaByteCodeGen
cp -r ../pikaCompiler/libpikabinder .
rm -r pikascript/pikascript-core
cp -r ../../src pikascript/pikascript-core
touch pikascript/pikascript-core/keep
rm -rf build
mkdir build
cd build
cmake ..
make -j
cp pikaByteCodeGen $ROOT/rust-msc-latest-linux
cd $ROOT
chmod +x rust-msc-latest-linux
strip -s rust-msc-latest-linux
upx -9 rust-msc-latest-linux
