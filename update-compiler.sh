ROOT=$PWD
cd pikascript-compiler-rust
cargo build
cd $ROOT
cp pikascript-compiler-rust/target/debug/rust-msc src/package/pikascript/rust-msc-latest-linux
cp pikascript-compiler-rust/target/debug/rust-msc.exe src/package/pikascript/rust-msc-latest-win10.exe
