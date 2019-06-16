FROM ubuntu:19.04
LABEL maintainer "Chris Ohk <utilforever@gmail.com>"

RUN apt-get update && apt-get install -y \
    build-essential \
    libcurl4-openssl-dev \
    cmake \
    gcc-8 \
    g++-8 \
    --no-install-recommends \
    && rm -rf /var/lib/apt/lists/*

COPY . /app

WORKDIR /app/build
RUN cmake .. -DCMAKE_C_COMPILER=gcc-8 -DCMAKE_CXX_COMPILER=g++-8 && \
    make -j "$(nproc)" && \
    make install
    
WORKDIR /