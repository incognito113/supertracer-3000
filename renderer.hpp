#include <shape.hpp>
#include <vector.hpp>
#pragma once

void render(int width = 512, int height = 512, point camera = (0.5, -1.0, 0.5), point light = (0.0, -0.5, 1.0), color background = (135, 206, 235));
// width and height are the pixel dimentions of the output image file
// camera is a point where the camera should be in the scene
// light is a point where the light should be in the scene
// background is the color of the skybox when there is no object
