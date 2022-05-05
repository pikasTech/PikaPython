cargo build --release
cbindgen --config cbindgen.toml --crate rust-msc --output pika_binder.h
