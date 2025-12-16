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

ENV PATH=/opt/riscv32-glibc/bin:$PATH
