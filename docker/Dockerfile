FROM ubuntu:22.04

ADD ubuntu22_sources.list /etc/apt/sources.list
RUN touch /etc/apt/apt.conf.d/99verify-peer.conf \
&& echo >>/etc/apt/apt.conf.d/99verify-peer.conf "Acquire { https::Verify-Peer false }"


RUN echo 'Asia/Shanghai' >/etc/timezone

# Update new packages
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt install -y tzdata
RUN apt-get install -y \
    build-essential \
    make \
    g++ \
    gcc \
    git \
    vim \
    cmake \
    sudo \
    curl \
    python3 \
    pip \
    cloc \
    wget

RUN apt install -y openssh-server

RUN git clone --branch v1.6.1 https://gitee.com/mirrors/google-benchmark benchmark && \
        cd benchmark && \
        git clone --branch release-1.11.0 https://gitee.com/mirrors/googletest && \
        cmake -E make_directory "build" && \
        cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../ && \
        cmake --build "build" --config Release -j && \
        sudo cmake --build "build" --config Release --target install -j
      

# Get Rust
# change source of Rust
ADD rust_config /root/.cargo/config
ENV RUSTUP_DIST_SERVER=https://mirrors.ustc.edu.cn/rust-static
ENV RUSTUP_UPDATE_ROOT=https://mirrors.ustc.edu.cn/rust-static/rustup
ENV PATH="/root/.cargo/bin:${PATH}"
RUN curl https://sh.rustup.rs -sSf | bash -s -- -y && \
    rustup target add i686-pc-windows-gnu


RUN apt-get install -y \
    ninja-build \
    valgrind \
    mingw-w64 \
    mingw-w64-tools  \
    mingw-w64-i686-dev  \
    mingw-w64-x86-64-dev  \
    wine \
    cbindgen

# only need on ubuntu20.04
# RUN dpkg --add-architecture i386 && apt-get update && apt-get install wine32 -y

RUN apt-get install -y golang
RUN go env -w GO111MODULE=on
RUN go env -w GOPROXY=https://goproxy.cn


RUN git clone https://gitee.com/lyon1998/pikascript  root/pikascript 
RUN cd /root/pikascript && git pull

RUN apt-get install -y upx
RUN cd /root/pikascript/tools/pikaCompiler && cargo build

ADD .vimrc /root/.vimrc

ADD bashrc /root/.bashrc
RUN apt-get install -y liblua5.3-dev
RUN apt-get install -y lcov

RUN cd root/pikascript && git remote add github https://github.com/pikastech/pikascript
RUN apt-get install clangd -y
RUN apt-get install libmbedtls-dev -y
RUN pip install -r /root/pikascript/docker/requirements.txt
RUN apt-get install clang-format -y

# install frps
RUN cd /root && wget https://gitee.com/Lyon1998/pikapython/attach_files/1420660/download -O frp.tar.gz && \
    tar -xzvf frp.tar.gz && \
    mv frp_0.48.0_linux_amd64/frpc /usr/local/bin/frpc

ADD sshd_config /etc/ssh/sshd_config
COPY frpc.ini /etc/frpc.ini
COPY frpc.sh /etc/frpc.sh
