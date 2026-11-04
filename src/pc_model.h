#pragma once
#include "scene.h"

namespace pcmodel {

    inline Material metal(glm::vec3 c, float shin = 96, float spec = 0.9f) { Material m; m.color = c; m.shininess = shin; m.specular = spec; return m; }
    inline Material plastic(glm::vec3 c) { Material m; m.color = c; m.shininess = 24; m.specular = 0.25f; return m; }
    inline Material glow(glm::vec3 c, glm::vec3 e) { Material m; m.color = c; m.emissive = e; m.shininess = 8; m.specular = 0.1f; return m; }

    inline Node* addFan(Node* parent, const std::string& name, const Mesh* box, const Mesh* cyl,
        glm::vec3 pos, int axis, float size, glm::vec3 explode, float delay, glm::vec3 led) {
        Node* fan = parent->add(name, nullptr, pos, glm::vec3(1), Material());
        fan->explode = explode; fan->explodeDelay = delay;
        if (axis == 1) { fan->rotAxis = { 0, 0, 1 }; fan->rotAngleDeg = -90.0f; }
        if (axis == 2) { fan->rotAxis = { 1, 0, 0 }; fan->rotAngleDeg = 90.0f; }

        fan->add(name + " frame", box, { 0, 0, 0 }, { size, 0.12f, size }, plastic({ 0.08f, 0.08f, 0.1f }));

        Node* rotor = fan->add(name + " rotor", cyl, { 0, 0.02f, 0 }, { 0.22f * size, 0.16f, 0.22f * size }, glow({ 0.1f, 0.1f, 0.12f }, led * 0.5f));
        rotor->spinSpeedDeg = 540.0f;
        for (int k = 0; k < 3; ++k) {
            Node* holder = rotor->add(name + " blade holder", nullptr, { 0, 0, 0 }, glm::vec3(1), Material());
            holder->rotAxis = { 0, 1, 0 }; holder->rotAngleDeg = 120.0f * k;

            Node* blade = holder->add(name + " blade", box, { 0.22f * size, 0, 0 },
                { 0.40f * size, 0.02f, 0.16f * size }, plastic({ 0.45f, 0.47f, 0.55f }));
            blade->rotAxis = { 1, 0, 0 }; blade->rotAngleDeg = 22.0f;
        }

        const int N = 20;
        for (int i = 0; i < N; ++i) {
            float a = 6.2831853f * i / N;
            Node* seg = fan->add(name + " led", box, { 0.46f * size * std::cos(a), 0.065f, 0.46f * size * std::sin(a) },
                { 0.08f * size, 0.02f, 0.05f * size }, glow(led * 0.3f, led));
            seg->rotAxis = { 0, 1, 0 }; seg->rotAngleDeg = -a * 57.29578f + 90.0f;
        }
        return fan;
    }