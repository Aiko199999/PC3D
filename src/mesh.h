//procedural meshes (box, cylinder)(Assylzhan)
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cmath>
#include <vector>
class Mesh {
public:
Mesh(const std::vector<float>& vertices,
const std::vector<unsigned>& indices) {
count = static_cast<GLsizei>(indices.size());
glGenVertexArrays(1, &vao);
glGenBuffers(1, &vbo);
glGenBuffers(1, &ebo);
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), indices.data(), GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
glEnableVertexAttribArray(1);
glBindVertexArray(0);
}
~Mesh() {
glDeleteVertexArrays(1, &vao);
glDeleteBuffers(1, &vbo);
glDeleteBuffers(1, &ebo);
}
Mesh(const Mesh&) = delete;
Mesh& operator=(const Mesh&) = delete;

void draw() const {
glBindVertexArray(vao);
glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
glBindVertexArray(0);
}

static Mesh* makeBox() {
struct Face { glm::vec3 n, u, v; };
const Face faces[6] = {
{{ 1, 0, 0}, {0, 0,-1}, {0, 1, 0}},
{{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}},
{{ 0, 1, 0}, {1, 0, 0}, {0, 0,-1}},
{{ 0,-1, 0}, {1, 0, 0}, {0, 0, 1}},
{{ 0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
{{ 0, 0,-1}, {-1,0, 0}, {0, 1, 0}},
};
std::vector<float> vtx;
std::vector<unsigned> idx;
for (const Face& f : faces) {
glm::vec3 c = f.n * 0.5f;
glm::vec3 p[4] = {
c - f.u * 0.5f - f.v * 0.5f,
c + f.u * 0.5f - f.v * 0.5f,
c + f.u * 0.5f + f.v * 0.5f,
c - f.u * 0.5f + f.v * 0.5f,
};
unsigned base = static_cast<unsigned>(vtx.size() / 6);
for (auto& q : p) {
vtx.insert(vtx.end(), {q.x, q.y, q.z, f.n.x, f.n.y, f.n.z});
}
idx.insert(idx.end(), {base, base + 1, base + 2, base, base + 2, base + 3});
}
return new Mesh(vtx, idx);
}

static Mesh* makeCylinder(int segments = 32) {
std::vector<float> vtx;
std::vector<unsigned> idx;
const float PI = 3.14159265358979f;
for (int i = 0; i <= segments; ++i) {
float a = 2.0f * PI * i / segments;
float cx = std::cos(a), cz = std::sin(a);
vtx.insert(vtx.end(), {0.5f * cx, -0.5f, 0.5f * cz, cx, 0, cz}); 
vtx.insert(vtx.end(), {0.5f * cx, 0.5f, 0.5f * cz, cx, 0, cz}); 
}
for (int i = 0; i < segments; ++i) {
unsigned b0 = 2 * i, t0 = 2 * i + 1, b1 = 2 * (i + 1), t1 = 2 * (i + 1) + 1;
idx.insert(idx.end(), {b1, b0, t0, b1, t0, t1});
}
auto cap = & {
unsigned centre = static_cast<unsigned>(vtx.size() / 6);
vtx.insert(vtx.end(), {0, y, 0, 0, ny, 0});
unsigned ring = static_cast<unsigned>(vtx.size() / 6);
for (int i = 0; i <= segments; ++i) {
float a = 2.0f * PI * i / segments;
vtx.insert(vtx.end(), {0.5f * std::cos(a), y, 0.5f * std::sin(a), 0, ny, 0});
}
for (int i = 0; i < segments; ++i) {
if (ny > 0) idx.insert(idx.end(), {centre, ring + i + 1, ring + i});
else idx.insert(idx.end(), {centre, ring + i, ring + i + 1});
}
};
cap(0.5f, 1.0f);
cap(-0.5f, -1.0f);
return new Mesh(vtx, idx);
}

private:
GLuint vao = 0, vbo = 0, ebo = 0;
GLsizei count = 0;
};
