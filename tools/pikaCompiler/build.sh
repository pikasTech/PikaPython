ROOT=$PWD
rm libpikabinder -rf
mkdir libpikabinder
cargo build --release
cbindgen --config cbindgen.toml --crate rust-msc --output libpikabinder/libpikabinder.h
cp target/release/libpikabinder.a libpikabinder

cd ../pikaByteCodeGen
cp ../pikaCompiler/libpikabinder . -r
rm pikascript/pikascript-core -r
cp ../../src pikascript/pikascript-core -r
touch pikascript/pikascript-core/keep
rm build -rf
mkdir build
cd build
cmake ..
make -j
cp pikaByteCodeGen $ROOT/rust-msc-latest-linux
cd $ROOT
chmod +x rust-msc-latest-linux
