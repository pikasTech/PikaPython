rm libout -rf
mkdir libout
cargo build --release
cbindgen --config cbindgen.toml --crate rust-msc --output libout/libpikabinder.h
cp target/release/libpikabinder.a libout
