# supertracer-3000

This is a ray tracing engine that allows users to create custom scenes with a variety of predefined solids, imported meshes via .obj files, and real-time camera controls. The code base was developed as the final project for CSCI 221 in Fall 2025.

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

### Bounding Volume Hierarchy

Our program uses a [bounding volume hierarchy](https://en.wikipedia.org/wiki/Bounding_volume_hierarchy) (BVH) to optimize ray intersections. Almost like a 3-dimensional binary search tree, the BVH intelligently splits all of the objects in the scene in half into two groups of shapes, each with a unique bounding box. These bounding boxes make it easy to check whether or not a given ray will intersect with any of the objects inside of it. We do this recursively so that with each bounding box calculation, we can split the number of objects remaining to check in half. The BVH makes calculating intersections blazingly fast, allowing for ultra-high-resolution and real-time rendering.

## Usage

### Interactability

Users can create their own scenes by interacting with `main.cpp`, and calling methods of the `Scene` class in order to add new shapes. Two demos are included for reference: a scene containing basic spheres and a plane, as well as the [Utah Teapot](https://en.wikipedia.org/wiki/Utah_teapot) imported from a .obj file. The camera can also be moved around real-time using either arrow keys or WASD, and E/Q to move in/out. Use the mouse or touchpad to pan/tilt, and scroll to zoom.

### Basic Classes

- Vectors can be initialized with `Vector(double x, double y, double z)`. They are used for positions, where (x, y, z) denote respective coordinates in Euclidean space. Vectors are also used for directions, where (x, y, z) represent a point relative to the origin (0, 0, 0); the difference between the origin and the point is the direction.

- Colors can be initialized with `Color(int red, int green, int blue)` in typical 0-255 RGB. Alternately, use `Color(double red, double green, double blue)` wherein each RGB value lies between 0-1.

- The `Material` class bundles all of the visual properties of objects in the scene. It can be initialized with `Material(Color color, double reflectivity)`. Color we've already covered, and is pretty self-explanatory. Reflectivity falls between 0-1, and sets how shiny the object should be: higher will be more reflective, while lower will be more matte.

### Setting the Scene Defaults

First things first, we will need a camera. The camera must be initialized with three variables: its position in the scene, the direction it's pointing in, and its field of view (FOV).
```cpp
void setCamera(const Vector pos, const Vector dir, const double fovDeg);
```

Then we will want to set the background color. If there aren't any objects in a certain part of the camera's FOV, the renderer will set that part of the image to this color.
```cpp
void setBackground(const int r, const int g, const int b);
```

Then we said, “Let there be light”; and there was light. In order for shapes to be anything other than black silhouttes, we'll need to put a light source somewhere in the scene. You can add as many of these as your heart desires.
```cpp
void addLight(const Vector pos, const Color color);
```

If we're not trying to emulate a film noire look, adding some ambient light can help make sure shadows aren't entirely pitch black.
```cpp
void setAmbientLight(const double ambient);
```

### Adding shapes

Now onto the fun part: shapes! Planes are defined by a point and a normal. The point can be any point that the plane will intersect with, and the normal vector points directly perpendicular (90 degrees) from the face of the plane.
```cpp
void addPlane(const Vector& point, const Vector& normal, const Material& mat);
```

Spheres are even easier: set the position of its midpoint with the `center` argument, and its radius with the `radius` argument.
```cpp
void addSphere(const Vector& center, double radius, const Material& mat);
```

Cylinders start to get more complex. Similarly to spheres, they are placed in Euclidean space by their midpoint `center`. The `radius` in this case is strictly for the horizontal dimension, while the vertical length of the cylinder is denoted with `height`.
```cpp
void addCylinder(const Vector& center, double radius, double height, const Material& material);
```

Triangles are simply defined by their three vertices.
```cpp
void addTriangle(const Vector& a, const Vector& b, const Vector& c, const Material& mat);
```

If you don't want to go through the tedious work of creating hundreds of triangles to make a mesh, we can do that for you! Place any .obj file of your choosing in the program's home directory, and then call `importOBJ` to load it into your scene. `offset` allows you to move your object around, and `scale` will multilpy each of the triangles by some constant. Set `scale = 0.5` to shrink it by half, or `scale = 2` to make it twice as large.
```cpp
bool importOBJ(const Vector& offset, const std::string fileName, const double scale, const Material& material);
```

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
