FROM ubuntu@sha256:8feb4d8ca5354def3d8fce243717141ce31e2c428701f6682bd2fafe15388214 AS build

ARG DEBIAN_FRONTEND=noninteractive
ARG QEMU_COMMIT=5ae4df8a7267149f6ff5a9f8fa4f6b8adbc28fbc
ARG QEMU_ARCHIVE_SHA256=964752b018267d6a30405ec026ca3f41e481774e3e45ec02b084893cc18945c5

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        ca-certificates \
        curl \
        libfdt-dev \
        libglib2.0-dev \
        libpixman-1-dev \
        libtool \
        pkg-config \
        python2 \
        zlib1g-dev \
    && ln -s /usr/bin/python2 /usr/local/bin/python \
    && rm -rf /var/lib/apt/lists/*

RUN curl --fail --location --retry 3 \
        "https://codeload.github.com/beckus/qemu_stm32/tar.gz/${QEMU_COMMIT}" \
        --output /tmp/qemu-stm32.tar.gz \
    && echo "${QEMU_ARCHIVE_SHA256}  /tmp/qemu-stm32.tar.gz" \
        | sha256sum --check - \
    && mkdir /tmp/qemu-stm32 \
    && tar -xzf /tmp/qemu-stm32.tar.gz \
        --strip-components=1 -C /tmp/qemu-stm32 \
    && cd /tmp/qemu-stm32 \
    && ./configure \
        --prefix=/opt/qemu-stm32 \
        --disable-xen \
        --disable-werror \
        --target-list=arm-softmmu \
    && make -j2 \
    && make install

FROM ubuntu@sha256:8feb4d8ca5354def3d8fce243717141ce31e2c428701f6682bd2fafe15388214

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        libfdt1 \
        libglib2.0-0 \
        libpixman-1-0 \
        libstdc++6 \
        libuuid1 \
        zlib1g \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/qemu-stm32 /opt/qemu-stm32

ENV PATH="/opt/qemu-stm32/bin:${PATH}"
LABEL org.opencontainers.image.source="https://github.com/beckus/qemu_stm32" \
      org.opencontainers.image.version="2.1.3" \
      org.opencontainers.image.revision="5ae4df8a7267149f6ff5a9f8fa4f6b8adbc28fbc" \
      io.pikapython.qemu.machine="stm32-p103"
ENTRYPOINT ["/opt/qemu-stm32/bin/qemu-system-arm"]
