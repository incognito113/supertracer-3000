// #include <vector>
// #include <string>
// #include <fstream>
// #include <iostream>

// #include "math/vector.hpp"
// #include "triangle.hpp"
// #include "scene/scene.hpp"

// bool importMesh(const Scene scene, const std::string fileName, const Material
// material) {
//     ifstream file(fileName);
//     if (!file.isopen()) {
//         cerr << "Error: file does not exist!";
//         return false;
//     }
//     std::vector<Vector> vertexList;
//     std::vector<Vector> faceVertices;
//     std::string currentLine;
//     std::string coordinate1;
//     std::string coordinate2;
//     std::string coordinate3;
//     std::string vertexIndex;
//     int splittingVertex;
//     while (getline(file, currentLine)) {
//         if (currentLine[0] == 'v') {
//             // gets a vertex and adds it to our big list of vertices
//             getLine(currentLine, coordinate1, ' '); // just the 'v'
//             getLine(currentLine, coordinate1, ' '); // first coord
//             getLine(currentLine, coordinate2, ' '); // second coord
//             getLine(currentLine, coordinate3, ' '); // third coord
//             vertexList.push_back(Vector((float) coordinate1, (float)
//             coordinate2, (float) coordinate3));
//         }
//         else if (currentLine[0] == 'f') {
//             // gets all of the vertices in the current face and loads them
//             into their own vector faceVertices.clear(); getLine(currentLine,
//             vertexIndex, ' '); // skips the leading 'f' while
//             (getLine(currentLine, vertexIndex, ' ')) {
//                 // while loop gets next block of vertex/texture/normal
//                 getLine(vertexIndex, vextexIndex, '/'); // gets just the
//                 vertex from the block (we don't need anything else)
//                 faceVertices.push_back(vertexList[(int) vertexIndex]); //
//                 adds the indexed vertex the list of vertices for this face
//             }
//             // splits the face (which can be any polygon) into a bunch of
//             triangles splittingVertex = 2; while (splittingVertex <
//             faceVertices.size()) {
//                 scene.addTriangle(faceVertices[0],
//                 faceVertices[splittingVertex - 1],
//                 faceVertices[splittingVertex], material); splittingVertex ++;
//             }
//         }
//     }

//     this->material = material;
//     this->mesh = faceList;
//     return true;
// }
