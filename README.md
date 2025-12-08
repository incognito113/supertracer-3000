# supertracer-3000

Ray tracing project for CSCI 221

### Requires SDL2 Library

On MacOS:
```console
brew install sdl2
```

On Linux w/ DNF:
```console
dnf install SDL2-devel
```

On other Linux distros: good luck!

### Optional Dependencies

To install metal-cpp on mac, run:

```console
sudo mkdir -p /Library/Developer/metal-cpp && TMP=$(mktemp -d) && curl -L -o "$TMP/metal-cpp.zip" https://developer.apple.com/metal/cpp/files/metal-cpp_26.zip && unzip -q "$TMP/metal-cpp.zip" -d "$TMP" && sudo cp -r "$TMP/metal-cpp/"* /Library/Developer/metal-cpp/ && rm -rf "$TMP"
```

### Makefile Commands

make: Compile both main and test executables

make main: Run main executable

make test: Run all tests

make leaks-main: Check for leaks in main

make leaks-test: Check for leaks in test
