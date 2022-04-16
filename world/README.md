# Rustymon World Generator

## Building

Besides `cmake`, the following packages are required
to build the world generator on Debian-like systems
(adapt the names to your system accordingly):

- `libboost-dev`
- `libbz2-dev`
- `libssl-dev`
- `libjsoncpp-dev`
- `libprotozero-dev`

On some systems, the following hack might be needed to properly
include the JSON C++ library during the build:

```sh
sudo ln -s /usr/include/jsoncpp/json /usr/include/json
```

After cloning this repository, make sure to initialize its submodules:

```sh
git submodule init
git submodule update
```

Then, proceed like with other CMake-based projects:

```sh
cd world
mkdir build
cd build
cmake ..
make
```

This should produce a single binary in the `build` directory.

## Executing

The binary is dynamically linked, and therefore requires
some runtime libraries present on the target machine:

- `libpthread.so.0`
- `libz.so.1`
- `libexpat.so.1`
- `libbz2.so.1.0`
- `libjsoncpp.so.24`
- `libstdc++.so.6`
- `libm.so.6`
- `libgcc_s.so.1`
- `libc.so.6`
