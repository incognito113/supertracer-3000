#include "renderer.hpp"

#include <fstream>

#include "color.hpp"
#include "tracer.hpp"

// Set camera parameters
void Scene::setCamera(const Vector pos, const Vector dir,
                      const double fov_deg) {
  camera.position = pos;
  camera.direction = dir;
  camera.fov = fov_deg;
}

// Set background color
void Scene::setBackground(const int r, const int g, const int b) {
  background = Color(r, g, b);
}

// Add a light to the scene
void Scene::addLight(const Vector pos, const Color color) {
  lights.push_back(Light{pos, color});
}

// Add a shape to the scene
void Scene::addShape(std::unique_ptr<Shape> shape) {
  shapes.push_back(std::move(shape));
}

// Trace a ray through the scene and return the resulting color
Color Scene::traceRay(const Ray& ray) const {
  // Placeholder implementation: return background color
  (void)ray;
  return background;
}

// Compute lighting for all lights at the hit point
Color Scene::computeLighting(const HitInfo& hitInfo) const {
  // Placeholder
  (void)hitInfo;
  return Color(1.0, 1.0, 1.0);
}

std::vector<Color> Scene::render_pixels() const {
  // Placeholder
  std::vector<Color> pixels;
  pixels.reserve(width * height);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      pixels.push_back(background);
    }
  }
  return pixels;
}

// Render the scene to a ppm file
void Scene::render(const std::string& filename) const {
  std::vector<Color> pixels = render_pixels();
  std::ofstream output(filename);
  if (!output.is_open()) {
    throw std::runtime_error("Failed to open output file: " + filename);
  }

  // PPM format:
  // P3
  // <width> <height>
  // 255
  // r g b r g b r g b ...
  output << "P3" << std::endl;  // ASCII format
  output << width << " " << height << std::endl;
  output << "255" << std::endl;
  for (const Color& color : pixels) {
    output << color.get255String() << " ";
  }
  output << std::endl;
  output.close();
}
