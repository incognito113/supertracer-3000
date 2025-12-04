#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "math/vector.hpp"
#include "scene/scene.hpp"
#include "shapes/triangle.hpp"

bool Scene::importOBJ(const Vector& offset, const std::string fileName,
                      const double scale, const Material& material) {
  std::ifstream file(fileName);
  if (!file.is_open()) {
    std::cerr << "Error: file does not exist!";
    return false;
  }

  std::vector<Vector> vertexList;
  std::vector<Vector> normalList;
  std::vector<Vector> faceVertices;
  std::vector<Vector> faceNormals;
  std::string currentLineString;
  std::stringstream currentLine;
  std::string x;
  std::string y;
  std::string z;
  std::stringstream vertexIndexStream;
  std::string vertexIndex;
  std::string textureIndex;  // unused but needed to parse correctly
  std::string normalIndex;
  Vector v1;
  Vector v2;
  Vector v3;
  Vector n1;
  Vector n2;
  Vector n3;
  Vector zero;

  while (std::getline(file, currentLineString)) {
    currentLine.str(currentLineString);
    currentLine.clear();

    // Skip empty lines or lines with '#' comments
    if (currentLineString.empty() || currentLineString[0] == '#') {
      continue;
    }

    if (currentLineString[0] == 'v') {
      // gets a vertex and adds it to our big list of vertices
      std::getline(currentLine, x, ' ');  // just the 'v' which we'll overwrite
      std::getline(currentLine, x, ' ');  // first coord
      std::getline(currentLine, y, ' ');  // second coord
      std::getline(currentLine, z, ' ');  // third coord

      if (currentLineString[1] == 'n') {
        // Normal vector ('vn')
        normalList.push_back(Vector(std::stod(x), std::stod(y), std::stod(z)));
      } else if (currentLineString[1] == ' ') {
        // Regular vertex ('v ')
        vertexList.push_back(Vector(std::stod(x), std::stod(y), std::stod(z)));
      }
    } else if (currentLineString[0] == 'f') {
      // Gets all vertices in the current face and load them into faceVertices
      faceVertices.clear();
      faceNormals.clear();
      std::getline(currentLine, vertexIndex, ' ');  // skip leading 'f'

      while (std::getline(currentLine, vertexIndex, ' ')) {
        vertexIndexStream.clear();
        vertexIndexStream << vertexIndex;

        // Load up to the first '/' (if any)
        std::getline(vertexIndexStream, vertexIndex, '/');
        // Load up to the second '/' (if any) - we ignore texture coords
        if (!std::getline(vertexIndexStream, textureIndex, '/')) {
          textureIndex = "";
        }
        // Load normal index (if any)
        if (!std::getline(vertexIndexStream, normalIndex, '/')) {
          normalIndex = "";
        }

        // Add normal and vertex to this face
        faceVertices.push_back(vertexList[stoi(vertexIndex) - 1]);
        if (normalIndex.empty()) {
          faceNormals.push_back(Vector());  // placeholder zero normal
        } else {
          faceNormals.push_back(normalList[stoi(normalIndex) - 1]);
        }
      }

      // splits the polygonal face into triangles
      int splittingVertex = 2;
      while ((size_t)splittingVertex < faceVertices.size()) {
        // Load triangle vertices and normals
        v1 = faceVertices[0] * scale + offset;
        v2 = faceVertices[splittingVertex - 1] * scale + offset;
        v3 = faceVertices[splittingVertex] * scale + offset;

        n1 = faceNormals[0];
        n2 = faceNormals[splittingVertex - 1];
        n3 = faceNormals[splittingVertex];

        // If any normal is zero, don't use face normals
        if (n1 == zero || n2 == zero || n3 == zero) {
          addTriangle(v1, v2, v3, material);
        } else {
          addTriangle(v1, v2, v3, n1, n2, n3, material);
        }

        splittingVertex++;
      }
    }
  }
  return true;
}
