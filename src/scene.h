// hierarchical scene graph + exploded-view logic(Assylzhan)
#pragma once
#include "mesh.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

struct Material {
glm::vec3 color{0.7f};
float alpha = 1.0f;
float shininess = 32.0f;
float specular = 0.4f;
glm::vec3 emissive{0.0f};
};

class Node {
public:
std::string name;
const Mesh* mesh = nullptr; // nullptr = pure grouping node
glm::vec3 pos{0.0f}; // local translation
glm::vec3 scale{1.0f}; // scale of THIS node's mesh only (children are not scaled)
glm::vec3 rotAxis{0, 1, 0};
float rotAngleDeg = 0.0f;
float spinSpeedDeg = 0.0f; // continuous rotation about local Y (fans)
float spinAngleDeg = 0.0f;
glm::vec3 explode{0.0f}; // displacement (parent space) at full explosion
float explodeDelay = 0.0f; // 0..1, parts with larger delay start moving later
Material mat;
std::vector<std::unique_ptr<Node>> children;

Node* add(const std::string& n, const Mesh* m, glm::vec3 p, glm::vec3 s, const Material& material) {
children.push_back(std::make_unique<Node>());
Node* c = children.back().get();
c->name = n; c->mesh = m; c->pos = p; c->scale = s; c->mat = material;
return c;
}
};

class Scene {
public:

};
