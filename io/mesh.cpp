#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "math/vector.hpp"
#include "scene/scene.hpp"
#include "shapes/triangle.hpp"

std::vector<std::string> splitByChar(std::string string, char splitter) {
  std::vector<std::string> blocks;
  std::string currentBlock;
  for (int i = 0; (size_t) i < string.size(); i++) {
    if (string[i] == splitter) {
      blocks.push_back(currentBlock);
      currentBlock.clear();
    } else {
      currentBlock += string[i];
    }
  }
  blocks.push_back(currentBlock);
  return blocks;
}


bool Scene::importOBJ(const Vector& offset, const std::string fileName,
                      const double scale, const Material& material) {
  std::ifstream file(fileName);
  if (!file.is_open()) {
    std::cerr << "Error: file does not exist!";
    return false;
  }

  std::string currentLine;
  std::vector<std::string> commandList;
  std::vector<Vector> vertexList;
  std::vector<Vector> normalList;
  std::vector<Vector> faceVertices;
  std::vector<Vector> faceNormals;
  double x;
  double y;
  double z;
  int i;
  std::vector<std::string> faceBlock;
  int vertexIndex;
  int normalIndex;
  int splitVertex;
  Vector v1;
  Vector v2;
  Vector v3;
  Vector n1;
  Vector n2;
  Vector n3;
  Vector zero;

  while (std::getline(file, currentLine)) {

    // Skip empty lines or lines with '#' comments
    if (currentLine.empty() || currentLine[0] == '#') {
      continue;
    }

    commandList = splitByChar(currentLine, ' ');

    if (commandList[0] == "v") {
      // regular vertex
      x = std::stod(commandList[1]);
      y = std::stod(commandList[2]);
      z = std::stod(commandList[3]);
      vertexList.push_back(Vector(x, y, z));

    } else if (commandList[0] == "vn") {
      // normal vertex??? idk
      x = std::stod(commandList[1]);
      y = std::stod(commandList[2]);
      z = std::stod(commandList[3]);
      normalList.push_back(Vector(x, y, z));

    } else if (commandList[0] == "f") {
      // Gets all vertices in the current face and load them into faceVertices
      faceVertices.clear();
      faceNormals.clear();

      for (i = 1; (size_t) i < commandList.size(); i++) {

        faceBlock = splitByChar(commandList[i], '/');

        // add the current vertex to the list of vertices for this face
        vertexIndex = std::stoi(faceBlock[0]);
        faceVertices.push_back(vertexList[vertexIndex - 1]);

        // if there is a normal vector defined, add it to the list
        if (faceBlock.size() > 2) {
          normalIndex = std::stoi(faceBlock[2]);
          faceNormals.push_back(normalList[normalIndex - 1]);
        } else {
          faceNormals.push_back(Vector());  // placeholder zero normal
        }
      }

      // splits the polygonal face into triangles
      for (splitVertex = 2; (size_t)splitVertex < faceVertices.size(); splitVertex++) {
        // Load triangle vertices and normals
        v1 = faceVertices[0] * scale + offset;
        v2 = faceVertices[splitVertex - 1] * scale + offset;
        v3 = faceVertices[splitVertex] * scale + offset;

        n1 = faceNormals[0];
        n2 = faceNormals[splitVertex - 1];
        n3 = faceNormals[splitVertex];

        // If any normal is zero, don't use face normals
        if (n1 == zero || n2 == zero || n3 == zero) {
          addTriangle(v1, v2, v3, material);
        } else {
          addTriangle(v1, v2, v3, n1, n2, n3, material);
        }
      }
    }
  }
  return true;
}
