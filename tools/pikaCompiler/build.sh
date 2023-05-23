if [ -d "../../../pikalab" ]; then
    sh do_build.sh
fi

if [ ! -d "../../../pikalab" ]; then
    wget http://pikapython.com/download/rust-msc-latest-linux
fi
