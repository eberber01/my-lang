FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    ca-certificates \
    xz-utils \
    wget \
    qemu-user \
    qemu-user-static \
    cmake \
    ninja-build \
    git \
    build-essential

RUN wget -q https://github.com/riscv-collab/riscv-gnu-toolchain/releases/download/2025.11.27/riscv32-glibc-ubuntu-24.04-gcc.tar.xz \
 && tar -xJf riscv32-glibc-ubuntu-24.04-gcc.tar.xz -C /opt \
 && rm riscv32-glibc-ubuntu-24.04-gcc.tar.xz

ENV PATH=/opt/riscv/bin:$PATH

RUN git clone https://github.com/google/googletest.git /tmp/googletest \
 && cd /tmp/googletest \
 && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
 && cmake --build build --config Release \
 && cp build/lib/*.a /usr/lib \
 && cp -r googletest/include/gtest /usr/include/ \
 && rm -rf /tmp/googletest