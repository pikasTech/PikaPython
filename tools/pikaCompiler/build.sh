ROOT=$PWD
rm libpikabinder -rf
mkdir libpikabinder
cargo build --release
cbindgen --config cbindgen.toml --crate rust-msc --output libpikabinder/libpikabinder.h
cp target/release/libpikabinder.a libpikabinder
cp target/release/rust-msc rust-msc-latest-linux
chmod +x rust-msc-latest-linux

cd ../pikaByteCodeGen
cp ../pikaCompiler/libpikabinder . -r
cp ../../src pikascript/pikascript-core -r
rm build -rf
mkdir build
cd build
cmake ..
make -j
