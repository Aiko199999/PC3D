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

};