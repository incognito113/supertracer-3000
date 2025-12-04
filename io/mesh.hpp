#pragma once

#include <string>

#include "scene/material.hpp"
#include "scene/scene.hpp"

bool importMesh(const Scene* scene, const std::string fileName, const Material material);
// takes an input .obj file and loads all of its tris with user-defined material into a scene
