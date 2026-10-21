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
    const Mesh* mesh = nullptr;
    glm::vec3 pos{0.0f};
    glm::vec3 scale{1.0f};
    glm::vec3 rotAxis{0, 1, 0};
    float rotAngleDeg = 0.0f;
    float spinSpeedDeg = 0.0f;
    float spinAngleDeg = 0.0f;
    glm::vec3 explode{0.0f};
    float explodeDelay = 0.0f;
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
    std::unique_ptr<Node> root = std::make_unique<Node>();
    std::vector<Node*> inspectable;
    float explodeFactor = 0.0f;

    void update(float dt) {
        spinRecursive(root.get(), dt);
    }

    void draw(Shader& sh, const Node* highlighted, float pulse) const {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
        drawNode(sh, root.get(), glm::mat4(1.0f), false, highlighted, false, pulse);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        drawNode(sh, root.get(), glm::mat4(1.0f), true, highlighted, false, pulse);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

private:
    static float smooth(float x) { x = std::clamp(x, 0.0f, 1.0f); return x * x * (3.0f - 2.0f * x); }

    static void spinRecursive(Node* n, float dt) {
        n->spinAngleDeg += n->spinSpeedDeg * dt;
        for (auto& c : n->children) spinRecursive(c.get(), dt);
    }

    void drawNode(Shader& sh, const Node* n, const glm::mat4& parent, bool transparentPass,
                  const Node* highlighted, bool inHighlight, float pulse) const {
        float local = smooth((explodeFactor - n->explodeDelay) / (1.0f - n->explodeDelay));
        glm::vec3 p = n->pos + n->explode * local;

        glm::mat4 M = parent * glm::translate(glm::mat4(1.0f), p);
        if (n->rotAngleDeg != 0.0f) M = glm::rotate(M, glm::radians(n->rotAngleDeg), n->rotAxis);
        if (n->spinAngleDeg != 0.0f) M = glm::rotate(M, glm::radians(n->spinAngleDeg), glm::vec3(0, 1, 0));

        bool hl = inHighlight || (n == highlighted);

        if (n->mesh && ((n->mat.alpha < 1.0f) == transparentPass)) {
            glm::mat4 model = glm::scale(M, n->scale);
            sh.setMat4("uModel", model);
            sh.setMat3("uNormalMat", glm::mat3(glm::transpose(glm::inverse(model))));
            sh.setVec3("uColor", n->mat.color);
            sh.setFloat("uAlpha", n->mat.alpha);
            sh.setFloat("uShininess", n->mat.shininess);
            sh.setFloat("uSpecular", n->mat.specular);
            sh.setVec3("uEmissive", n->mat.emissive);
            sh.setFloat("uHighlight", hl ? pulse : 0.0f);
            n->mesh->draw();
        }
        for (auto& c : n->children)
            drawNode(sh, c.get(), M, transparentPass, highlighted, hl, pulse);
    }
};
