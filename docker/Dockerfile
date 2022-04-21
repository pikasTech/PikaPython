FROM ubuntu:20.04

RUN apt-get update
RUN apt-get install wget -y

RUN wget -O /etc/apt/sources.list http://mirrors.cloud.tencent.com/repo/ubuntu20_sources.list

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
    ninja-build \
    curl

RUN git clone --branch v1.6.1 https://gitee.com/mirrors/google-benchmark benchmark && \
        cd benchmark && \
        git clone https://gitee.com/mirrors/googletest && \
        cmake -E make_directory "build" && \
        cmake -E chdir "build" cmake -DBENCHMARK_DOWNLOAD_DEPENDENCIES=on -DCMAKE_BUILD_TYPE=Release ../ && \
        cmake --build "build" --config Release && \
        sudo cmake --build "build" --config Release --target install
      

# Get Rust
ENV RUSTUP_DIST_SERVER=https://mirrors.ustc.edu.cn/rust-static
ENV RUSTUP_UPDATE_ROOT=https://mirrors.ustc.edu.cn/rust-static/rustup
RUN curl https://sh.rustup.rs -sSf | bash -s -- -y

ENV PATH="/root/.cargo/bin:${PATH}"


RUN apt-get install -y python3
