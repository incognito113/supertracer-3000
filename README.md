# supertracer-3000

Ray tracing engine that allows users to create custom scenes with a variety of predefined solids, imported meshes via .obj files, and real-time camera controls. The code base was developed as the final project for CSCI 221 in Fall 2025.

## Table of Contents

- [Installation](#installation)
- [Requirements](#required-libraries)
    - [Linux](#linux)
    - [MacOS](#macos)
- [Design](#design)
- [Usage](#usage)
- [Contributors](#contributors)


## Installation

You must have [Git](https://git-scm.com/) in order to install the program.

Install supertracer-3000 with git:
```console
git clone https://github.com/incognito113/supertracer-3000
```


## Required Libraries

### Linux

#### 1. Users with Linux systems or WSL should use gcc. 

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

#### 2. The SDL2 library is also required.

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

#### 1. Supertracer-3000 is designed to work with clang on MacOS.

Make sure you have clang installed before proceeding:
```console
clang --version
```

If it is not already installed, please install it:
```console
xcode-select --install
```

#### 2. Also requires the SDL2 library.

Install with homebrew:
```console
brew install sdl2
```

#### 3. MacOS includes the experimental GPU-based rendering option, which requires metal-cpp.

> [!WARNING]
> This mode is experimental and may not work. If you want to try to get it working, install this. Otherwise, it is unnecessary. Proceed at your own risk.

Install metal-cpp on Mac:
```console
sudo mkdir -p /Library/Developer/metal-cpp && TMP=$(mktemp -d) && curl -L -o "$TMP/metal-cpp.zip" https://developer.apple.com/metal/cpp/files/metal-cpp_26.zip && unzip -q "$TMP/metal-cpp.zip" -d "$TMP" && sudo cp -r "$TMP/metal-cpp/"* /Library/Developer/metal-cpp/ && rm -rf "$TMP"
```


## Design

### Classes Overview

At the highest level, the `Scene` class contains all of the information for the render. This includes the position of the camera, any point lights, and all of the shapes to render. Shapes all inherit from a default `Shape` class. They are bundled with their color and reflectivity, and each unique shape type defines its intersection math. During tracing, this will determine which shape (if any) the ray collides with. Most of the code is based on the `Vector` class, which handles a lot of the low-level calculations. This allows code to easily be implemented while leaving the vector math to the `Vector` class.

### File Tree

`io/` contains all of the file interfacing code. This includes the ability to import .obj files, and exporting rendered .ppm files.

`math/` contains all of the low-level physics and math, including the definitions for our color, vector, and ray classes.

`renderer/` pulls everything else together to compute the interactions between the camera, light source, and all of the shapes in the scene.

`scene/` contains all of the other classes related to the scene, which holds everything that will be rendered, and properties of objects in the scene.

`shaders/` contains experimental code for running the tracer on the GPU.

`shapes/` includes code for calculating intersections of every type of object that can be rendered. Currently, these include planes, spheres, triangles, and cylinders.


## Usage

### User Interaction

Users can create their own scenes by interacting with `main.cpp`, and calling methods of the `Scene` class in order to add new shapes. Two demos are included for reference: a scene containing basic spheres and a plane, as well as the [Utah Teapot](https://en.wikipedia.org/wiki/Utah_teapot) imported from a .obj file.


### Makefile Commands

Compile and run the renderer for the scene defined in `main.cpp`:
```console
make main
```

#### The following Make commands should be unnecessary so long as you have only modified code within `main.cpp`.

Run tests of the underlying code:
```console
make test
```

Compile both main and test executables:
```console
make
```

Check for memory leaks in the main executable:
```console
make leaks-main
```

Check for memory leaks in the test executable:
```console
make leaks-test
```

## Contributors

Asa Karon worked on the .ppm exporter, .obj importer, and this beautiful readme.

Quincy Nash worked on ??????

Wystan Wu worked on some math foundations and the shape system. He is very grateful for the beautiful work and collaboration of the other group members.
