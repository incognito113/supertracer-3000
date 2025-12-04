#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "math/vector.hpp"
#include "triangle.hpp"
#include "scene/scene.hpp"

bool importMesh(const Scene& scene, const std::string fileName, const Material& material) {
    std::ifstream file(fileName);
    if (!file.isopen()) {
        std::cerr << "Error: file does not exist!";
        return false;
    }
    std::vector<Vector> vertexList;
    std::vector<Vector> faceVertices;
    std::string currentLine;
    std::string coord1;
    std::string coord2;
    std::string coord3;
    std::string vertexIndex;
    int splittingVertex;
    while (std::getline(file, currentLine)) {
        if (currentLine[0] == 'v') {
            // gets a vertex and adds it to our big list of vertices
            std::getline(currentLine, coord1, ' '); // just the 'v' which we'll overwrite
            std::getline(currentLine, coord1, ' '); // first coord
            std::getline(currentLine, coord2, ' '); // second coord
            std::getline(currentLine, coord3, ' '); // third coord
            vertexList.push_back(Vector((double) coord1, (double) coord2, (double) coord3));
        }
        else if (currentLine[0] == 'f') {
            // gets all of the vertices in the current face and loads them into their own vector
            faceVertices.clear();
            std::getline(currentLine, vertexIndex, ' '); // skips the leading 'f'
            while (std::getline(currentLine, vertexIndex, ' ')) {
                // while loop gets next block of vertex/texture/normal
                std::getline(vertexIndex, vextexIndex, '/'); // gets just the vertex from the block (we don't need anything else)
                faceVertices.push_back(vertexList[(int) vertexIndex]); // adds the indexed vertex the list of vertices for this face
            }
            // splits the face (which can be any polygon) into a bunch of triangles
            splittingVertex = 2;
            while (splittingVertex < faceVertices.size()) {
                scene->addTriangle(faceVertices[0], faceVertices[splittingVertex - 1], faceVertices[splittingVertex], &material);
                splittingVertex ++;
            }
        }
    }
    return true;
}
