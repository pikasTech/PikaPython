ROOT=$PWD

# clean
rm libpikabinder -rf
mkdir libpikabinder

cargo build --release --target=i686-pc-windows-gnu
cbindgen --config cbindgen.toml --crate rust-msc --output libpikabinder/libpikabinder.h
cp target/i686-pc-windows-gnu/release/libpikabinder.a libpikabinder

cd ../pikaByteCodeGen
cp ../pikaCompiler/libpikabinder . -r
cp ../../src pikascript/pikascript-core -r

sh cross-win.sh

cp pikaByteCodeGen $ROOT/rust-msc-latest-linux
cd $ROOT
chmod +x rust-msc-latest-linux
