FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
  && apt-get install -y --no-install-recommends \
  build-essential \
  g++ \
  libevdev-dev \
  libgtkmm-4.0-dev \
  libwayland-dev \
  libwayland-bin \
  libxtst-dev \
  meson pkg-config \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /src
