FROM ubuntu:20.04

ADD docker/ubuntu20_sources.list /etc/apt/sources.list

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
    python3

RUN git clone --branch v1.6.1 https://gitee.com/mirrors/google-benchmark benchmark && \
        cd benchmark && \
        git clone https://gitee.com/mirrors/googletest && \
        cmake -E make_directory "build" && \
        cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../ && \
        cmake --build "build" --config Release -j && \
        sudo cmake --build "build" --config Release --target install -j
      

# Get Rust
ENV RUSTUP_DIST_SERVER=https://mirrors.ustc.edu.cn/rust-static
ENV RUSTUP_UPDATE_ROOT=https://mirrors.ustc.edu.cn/rust-static/rustup
RUN curl https://sh.rustup.rs -sSf | bash -s -- -y

ENV PATH="/root/.cargo/bin:${PATH}"


RUN git config --global --add safe.directory /usr/src/pikascript
RUN git config --global user.email "liang6516@outlook.com"
RUN git config --global user.name "lyon"


RUN apt-get install -y \
    ninja-build \
    valgrind \
    mingw-w64 \
    mingw-w64-tools  \
    mingw-w64-i686-dev  \
    mingw-w64-x86-64-dev  \
    wine

RUN dpkg --add-architecture i386 && apt-get update && apt-get install wine32 -y


COPY . /pikascript
