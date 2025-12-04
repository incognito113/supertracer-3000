#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "math/vector.hpp"
#include "scene/scene.hpp"
#include "shapes/triangle.hpp"

bool Scene::importOBJ(const Vector& offset, const std::string fileName,
                      const Material& material) {
  std::ifstream file(fileName);
  if (!file.is_open()) {
    std::cerr << "Error: file does not exist!";
    return false;
  }

  std::vector<Vector> vertexList;
  std::vector<Vector> faceVertices;
  std::stringstream currentLine;
  std::string currentLineString;
  std::string x;
  std::string y;
  std::string z;
  std::stringstream vertexIndexStream;
  std::string vertexIndex;
  int splittingVertex;
  Vector v1;
  Vector v2;
  Vector v3;

  while (std::getline(file, currentLineString)) {
    currentLine.clear();
    currentLine << currentLineString;
    if (currentLineString[0] == 'v') {
      // gets a vertex and adds it to our big list of vertices
      std::getline(currentLine, x, ' ');  // just the 'v' which we'll overwrite
      std::getline(currentLine, x, ' ');  // first coord
      std::getline(currentLine, y, ' ');  // second coord
      std::getline(currentLine, z, ' ');  // third coord
      vertexList.push_back(Vector(std::stod(x), std::stod(y), std::stod(z)));
    } else if (currentLineString[0] == 'f') {
      // gets all of the vertices in the current face and loads them into their
      // own vector
      faceVertices.clear();
      std::getline(currentLine, vertexIndex, ' ');  // skips the leading 'f'
      while (std::getline(currentLine, vertexIndex, ' ')) {
        vertexIndexStream.clear();
        vertexIndexStream << vertexIndex;
        // while loop gets next block of vertex/texture/normal
        std::getline(vertexIndexStream, vertexIndex,
                     '/');  // gets just the vertex from the block (we don't
                            // need anything else)
        faceVertices.push_back(
            vertexList[stoi(vertexIndex)]);  // adds the indexed vertex the list
                                             // of vertices for this face
      }
      // splits the face (which can be any polygon) into a bunch of triangles
      splittingVertex = 2;
      while ((size_t)splittingVertex < faceVertices.size()) {
        v1 = faceVertices[0] + offset;
        v2 = faceVertices[splittingVertex - 1] + offset;
        v3 = faceVertices[splittingVertex] + offset;
        addTriangle(v1, v2, v3, material);
        splittingVertex++;
      }
    }
  }
  return true;
}
