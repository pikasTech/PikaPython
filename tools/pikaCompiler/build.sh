if [ -d "../../../pikalab" ]; then
    sh do_build.sh
fi

if [ ! -d "../../../pikalab" ]; then
    rm rust-msc-latest-linux
    wget http://pikapython.com/download/rust-msc-latest-linux
fi
