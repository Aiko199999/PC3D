//orbit camera + keyframe cinematic tour  (Baizhana)
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

class OrbitCamera {
public:
    float yaw = 0.7f;
    float pitch = 0.25f;
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

class CameraTour {
public:
    struct Key { glm::vec3 position; glm::vec3 target; };

    std::vector<Key> keys;
    float segmentDuration = 3.2f;
    bool active = false;
    float time = 0.0f;

    void start() { active = true; time = 0.0f; }
    void stop() { active = false; }
    float totalTime() const { return keys.size() > 1 ? segmentDuration * (keys.size() - 1) : 0.0f; }

    bool update(float dt, CameraPose& out) {
        if (!active || keys.size() < 2) return false;
        time += dt;
        float total = totalTime();
        bool finished = time >= total;
        float t = std::min(time, total);
        float u = t / segmentDuration;
        int i = std::min(static_cast<int>(u), static_cast<int>(keys.size()) - 2);
        float f = u - i;
        out.position = catmull(i, f, true);
        out.target = catmull(i, f, false);
        if (finished) active = false;
        return !finished;
    }

private:
    glm::vec3 get(int i, bool pos) const {
        i = std::clamp(i, 0, static_cast<int>(keys.size()) - 1);
        return pos ? keys[i].position : keys[i].target;
    }
    glm::vec3 catmull(int i, float t, bool pos) const {
        glm::vec3 p0 = get(i - 1, pos), p1 = get(i, pos), p2 = get(i + 1, pos), p3 = get(i + 2, pos);
        float t2 = t * t, t3 = t2 * t;
        return 0.5f * ((2.0f * p1) + (-p0 + p2) * t +
            (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
            (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
    }
};