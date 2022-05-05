rm libout -rf
mkdir libout
cargo build --release
cbindgen --config cbindgen.toml --crate rust-msc --output libout/libpikabinder.h
cp target/release/libpikabinder.a libout
cp target/release/rust-msc rust-msc-latest-linux
chmod +x rust-msc-latest-linux
