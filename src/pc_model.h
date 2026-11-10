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

    inline void build(Scene& scene, const Mesh* box, const Mesh* cyl) {
        Node* world = scene.root.get();

        // ground
        Node* ground = world->add("Ground", box, { 0, -0.15f, 0 }, { 40, 0.1f, 40 }, plastic({ 0.16f, 0.18f, 0.25f }));
        ground->explode = { 0, -2.2f, 0 };

        Node* pc = world->add("PC", nullptr, { 0, 0, 0 }, glm::vec3(1), Material());

        // chassis panels
        Material steel = metal({ 0.30f, 0.32f, 0.38f });
        Node* bottom = pc->add("Bottom panel", box, { 0, 0.1f, 0 }, { 2.2f, 0.06f, 4.4f }, steel);  bottom->explode = { 0, -1.2f, 0 };
        Node* top = pc->add("Top panel", box, { 0, 4.6f, 0 }, { 2.2f, 0.06f, 4.4f }, steel);  top->explode = { 0, 1.8f, 0 };
        Node* back = pc->add("Back panel", box, { 0, 2.35f, -2.2f }, { 2.2f, 4.5f, 0.06f }, steel); back->explode = { 0, 0, -2.8f };
        Node* front = pc->add("Front panel", box, { 0, 2.35f, 2.2f }, { 2.2f, 4.5f, 0.06f }, steel); front->explode = { 0, 0, 2.8f };
        pc->add("Front LED strip", box, { 0, 2.35f, 2.25f }, { 0.06f, 3.6f, 0.02f }, glow({ 0.1f, 0.1f, 0.15f }, { 0.2f, 0.6f, 1.0f }))->explode = { 0, 0, 2.8f };
        Node* left = pc->add("Left panel", box, { -1.1f, 2.35f, 0 }, { 0.06f, 4.5f, 4.4f }, steel); left->explode = { -3.0f, 0, 0 };
        Material glass; glass.color = { 0.55f, 0.75f, 0.9f }; glass.alpha = 0.22f; glass.shininess = 128; glass.specular = 1.0f;
        Node* glassP = pc->add("Tempered glass", box, { 1.1f, 2.35f, 0 }, { 0.05f, 4.5f, 4.4f }, glass); glassP->explode = { 3.4f, 0, 0 };

        // motherboard + children
        Node* mb = pc->add("Motherboard", box, { -0.85f, 2.75f, -0.275f }, { 0.06f, 3.05f, 2.45f }, plastic({ 0.04f, 0.30f, 0.16f }));
        mb->explode = { -1.2f, 0, 0 }; mb->explodeDelay = 0.30f;
        scene.inspectable.push_back(mb);

        mb->add("Socket", box, { 0.05f, 0.15f, -0.4f }, { 0.05f, 0.35f, 0.35f }, metal({ 0.6f, 0.6f, 0.62f }));
        mb->add("Chipset heatsink", box, { 0.08f, -0.8f, 0.45f }, { 0.1f, 0.5f, 0.5f }, metal({ 0.1f, 0.1f, 0.12f }))->explode = { 0.5f, 0, 0 };
        mb->add("VRM heatsink", box, { 0.08f, 1.15f, -0.95f }, { 0.1f, 0.55f, 0.5f }, metal({ 0.1f, 0.1f, 0.12f }))->explode = { 0.5f, 0, 0 };

        // CPU cooler(tower + fan)
        Node* cooler = mb->add("CPU Cooler", box, { 0.35f, 0.15f, -0.4f }, { 0.55f, 1.2f, 0.75f }, metal({ 0.55f, 0.57f, 0.6f }, 64, 0.8f));
        cooler->explode = { 1.9f, 0.3f, 0 }; cooler->explodeDelay = 0.45f;
        scene.inspectable.push_back(cooler);
        addFan(cooler, "CPU fan", box, cyl, { 0.38f, 0, 0 }, 1, 0.8f, { 0.5f, 0, 0 }, 0.0f, { 1.0f, 0.2f, 0.4f });

        // RAM
        for (int i = 0; i < 2; ++i) {
            Node* ram = mb->add(i == 0 ? "RAM 1" : "RAM 2", box, { 0.1f, 0.2f, 0.4f + 0.17f * i }, { 0.12f, 1.0f, 0.07f }, plastic({ 0.05f, 0.05f, 0.07f }));
            ram->explode = { 0.9f, 0.9f, 0 }; ram->explodeDelay = 0.5f;
            ram->add("RAM LED", box, { 0, 0.53f, 0 }, { 0.1f, 0.06f, 0.06f }, glow({ 0.2f, 0.2f, 0.2f }, { 0.2f, 1.0f, 0.5f }));
            if (i == 0) scene.inspectable.push_back(ram);
        }

        // GPU (parallel to board, fans facing the glass)
        Node* gpu = mb->add("GPU", box, { 0.4f, -0.95f, -0.05f }, { 0.5f, 0.95f, 2.7f }, metal({ 0.12f, 0.12f, 0.15f }, 80, 0.7f));
        gpu->explode = { 1.2f, -0.9f, 0 }; gpu->explodeDelay = 0.4f;
        scene.inspectable.push_back(gpu);
        addFan(gpu, "GPU fan 1", box, cyl, { 0.28f, 0.0f, -0.65f }, 1, 0.8f, { 0.4f, 0, 0 }, 0.0f, { 0.3f, 0.6f, 1.0f });
        addFan(gpu, "GPU fan 2", box, cyl, { 0.28f, 0.0f,  0.5f }, 1, 0.8f, { 0.4f, 0, 0 }, 0.0f, { 0.3f, 0.6f, 1.0f });
        gpu->add("GPU RGB bar", box, { 0.26f, 0.5f, 0.0f }, { 0.02f, 0.05f, 2.5f }, glow({ 0.2f, 0.2f, 0.3f }, { 0.5f, 0.3f, 1.0f }));

        // PSU
        Node* psu = pc->add("PSU", box, { 0.0f, 0.55f, -1.2f }, { 1.8f, 0.85f, 1.5f }, metal({ 0.1f, 0.1f, 0.11f }, 48, 0.5f));
        psu->explode = { 0, -1.6f, -1.0f }; psu->explodeDelay = 0.35f;
        scene.inspectable.push_back(psu);
        psu->add("PSU label", box, { 0.91f, 0.0f, 0.0f }, { 0.01f, 0.4f, 0.9f }, plastic({ 0.5f, 0.1f, 0.1f }));

        // case fans
        Node* f1 = addFan(pc, "Front fan 1", box, cyl, { 0.0f, 1.7f, 1.8f }, 2, 1.0f, { 0, 0, 1.6f }, 0.3f, { 0.2f, 0.8f, 1.0f });
        Node* f2 = addFan(pc, "Front fan 2", box, cyl, { 0.0f, 2.85f, 1.8f }, 2, 1.0f, { 0, 0, 1.6f }, 0.3f, { 0.2f, 0.8f, 1.0f });
        Node* f3 = addFan(pc, "Rear fan", box, cyl, { 0.0f, 3.5f, -1.85f }, 2, 1.0f, { 0, 0, -1.6f }, 0.3f, { 0.2f, 0.8f, 1.0f });
        scene.inspectable.push_back(f1); scene.inspectable.push_back(f2); scene.inspectable.push_back(f3);
        scene.inspectable.push_back(glassP);
    }

}