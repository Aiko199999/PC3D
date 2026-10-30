//orbit camera + keyframe cinematic tour (Baizhana)
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <vector>

struct CameraPose {
    glm::vec3 position;
    glm::vec3 target;
};

//mouse drag = rotate, right drag = pan, wheel = zoom
class OrbitCamera {
    public:
        float yaw = 0.7f;        // radians
        float pitch = 0.25f;     // radians
        float radius = 12.0f;
        glm::vec3 target{ 0.0f, 2.4f, 0.0f };

        glm::vec3 position() const {
            return target + radius * glm::vec3(std::cos(pitch) * std::sin(yaw),
                std::sin(pitch),
                std::cos(pitch) * std::cos(yaw));
        }
        void rotate(float dx, float dy) {
            yaw -= dx * 0.005f;
            pitch = std::clamp(pitch + dy * 0.005f, -1.5f, 1.5f);
        }
        void zoom(float amount) { radius = std::clamp(radius * (1.0f - amount * 0.08f), 2.0f, 40.0f); }
        void pan(float dx, float dy) {
            glm::vec3 fwd = glm::normalize(target - position());
            glm::vec3 right = glm::normalize(glm::cross(fwd, glm::vec3(0, 1, 0)));
            glm::vec3 up = glm::cross(right, fwd);
            float s = radius * 0.0015f;
            target += (-dx * right + dy * up) * s;
        }
        void setFromPose(const CameraPose& p) {
            glm::vec3 d = p.position - p.target;
            radius = glm::length(d);
            pitch = std::asin(std::clamp(d.y / radius, -1.0f, 1.0f));
            yaw = std::atan2(d.x, d.z);
            target = p.target;
        }
        CameraPose pose() const { return { position(), target }; }
    };
};