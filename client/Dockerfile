FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    cmake \
    g++-12 \
    make \
    curl \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt .
COPY common ./common
COPY client ./client

RUN cmake -S . -B build -DBUILD_CLIENT=ON -DBUILD_SERVER=OFF \
    -DCMAKE_C_COMPILER=/usr/bin/gcc-12 \
    -DCMAKE_CXX_COMPILER=/usr/bin/g++-12 && \
    cmake --build build --target client -j
