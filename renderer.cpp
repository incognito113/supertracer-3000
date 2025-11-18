#include <shape.hpp>
#include <plane.hpp>
#include <sphere.hpp>
#include <vector.hpp>
#include <fstream>
                                                                                                                // make smth for colors where it can translate from 24-bit color
void render(int width = 512, int height = 512, point camera = (0.5, -1.0, 0.5), point light = (0.0, -0.5, 1.0), color background = (135, 206, 235)){
    color background = (1, 1, 1); // color of the sky box

    ofstream output;
    output.open("output.ppm");
    output << "P3\n"; // means we're working in ASCII instead of binary
    output << width << " " << height << "\n"; // tells the file the width and height of the output image
    output << "255\n"; // max value for our RGBs

    int curRow;
    int curCol;
    color pixelRGB;
    point pixelPos;
    ray pixelRay;
    for (curRow = 0; curRow < height ; curRow++) { // top to bottom
        for (curCol = 0; curCol < width; curCol++) { // left to right

            // pixelPos = (curCol / width, 0, (height - curRow) / width)
            // pixelRay = (camera, pixelPos)

            // find object we intersect with

            // if (object = NULL)
                // color = background;

            // calculate the color at the point the ray hit the object (somehow) (look at other ppl's header files)

            output << (color.r * 255) << " " << (color.g * 255) << " " << (color.b * 255) << "\n";
        }
    }
    output.close()
}
