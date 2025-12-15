# supertracer-3000

Ray tracing engine that allows users to create custom scenes with a variety of predefined solids, imported meshes via .obj files, and real-time camera controls. The code base was developed as the final project for CSCI 221 in Fall 2025.

## Table of Contents

- [Installation](#installation)
- [Requirements](#required-libraries)
    - [Linux](#linux)
    - [MacOS](#macos)
- [Usage](#usage)
- [Contributors](#contributors)


## Installation

You must have [Git](https://git-scm.com/) in order to install the program.

Install with git:
```console
git clone https://github.com/incognito113/supertracer-3000
```


## Required Libraries

### Linux

#### Users with Linux systems or WSL should use gcc. 

Check to see if you have it installed with:
```console
gcc --version
```

If GCC is not installed, please install it with your respective package manager.
```console
dnf install g++
```

```console
apt-get install g++
```

```console
pacman -S gcc
```

#### The SDL2 library is also required.

Once again, install with your preferred package manager.
```console
dnf install SDL2-devel
```

```console
apt-get install libsdl2-2.0
```

```console
pacman -S sdl2
```


### MacOS

#### Supertracer-3000 is designed to work with clang on MacOS.

Make sure you have clang installed before proceeding:
```console
clang --version
```

If it is not already installed, please install it:
(ASA: DOES THIS WORK? IDK I DON'T HAVE A MAC. I JUST GUESSED.....)
```console
brew install clang
```

#### Also requires the SDL2 library.

Install with homebrew:
```console
brew install sdl2
```

<!---

#### Optional Dependencies

To install metal-cpp on mac, run:

```console
sudo mkdir -p /Library/Developer/metal-cpp && TMP=$(mktemp -d) && curl -L -o "$TMP/metal-cpp.zip" https://developer.apple.com/metal/cpp/files/metal-cpp_26.zip && unzip -q "$TMP/metal-cpp.zip" -d "$TMP" && sudo cp -r "$TMP/metal-cpp/"* /Library/Developer/metal-cpp/ && rm -rf "$TMP"
```

--->

## Usage

### Makefile Commands

make: Compile both main and test executables

make main: Run main executable

make test: Run all tests

make leaks-main: Check for leaks in main

make leaks-test: Check for leaks in test

## Contributors
