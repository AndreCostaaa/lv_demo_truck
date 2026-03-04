# BUILD ------------------------------------------------------------------------
FROM torizon/cross-toolchain-arm64-imx8:4 AS build

RUN dpkg --add-architecture arm64

RUN apt-get -q -y update && \
    apt-get -q -y install --no-install-recommends \
    curl git cmake file python3 python3-venv pkg-config \
    libevdev-dev:arm64 \
    wayland-protocols:arm64 \
    libdrm-dev:arm64 \
    libgbm-dev:arm64 \
    libwayland-dev:arm64 \
    libgles-dev:arm64 \
    libegl-dev:arm64 \
    libxkbcommon-dev:arm64 && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Compile using the cross-compiler provided by the base image
RUN ls && CC=aarch64-linux-gnu-gcc cmake -S . -B build-torizon -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build-torizon -j$(nproc)

# DEPLOY -----------------------------------------------------------------------
FROM --platform=linux/arm64 torizon/wayland-base-am62p:4 AS deploy

# Install RUNTIME dependencies only (no -dev packages here)
RUN apt-get -y update && apt-get install -y --no-install-recommends \
    libxkbcommon0 \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY --from=build /app/build-torizon/lv_demo_truck /usr/bin/lv_demo_truck
COPY ui/assets /assets

ENTRYPOINT ["/usr/bin/lv_demo_truck"]
CMD ["/assets"]
