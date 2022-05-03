c-utf8
======

UTF-8 Handling in Standard ISO-C11

The c-utf8 project implements utility functions around handling UTF-8 in
Standard ISO-C11. For API documentation, see the c-utf8.h header file, as well
as the docbook comments for each function.

### Project

 * **Website**: <https://c-util.github.io/c-utf8>
 * **Bug Tracker**: <https://github.com/c-util/c-utf8/issues>

### Requirements

The requirements for this project are:

 * `libc` (e.g., `glibc >= 2.16`)

At build-time, the following software is required:

 * `meson >= 0.60`
 * `pkg-config >= 0.29`

### Build

The meson build-system is used for this project. Contact upstream
documentation for detailed help. In most situations the following
commands are sufficient to build and install from source:

```sh
mkdir build
cd build
meson setup ..
ninja
meson test
ninja install
```

No custom configuration options are available.

### Repository:

 - **web**:   <https://github.com/c-util/c-utf8>
 - **https**: `https://github.com/c-util/c-utf8.git`
 - **ssh**:   `git@github.com:c-util/c-utf8.git`

### License:

 - **Apache-2.0** OR **LGPL-2.1-or-later**
 - See AUTHORS file for details.
