// PC3D

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "camera.h"
#include "mesh.h"
#include "scene.h"
#include "shader.h"

static void printHelp() {
    std::cout << "\PC3D controls\n"
                 "  Left mouse drag   rotate camera\n"
                 "  Right mouse drag  pan\n"
                 "  Mouse wheel       zoom\n"
                 "  E                 explode / assemble\n"
                 "  T                 start / stop cinematic tour\n"
                 "  N                 highlight next component (Shift+N previous)\n"
                 "  L                 toggle moving light\n"
                 "  R                 reset camera\n"
                 "  ESC               quit\n\n";
}


//callbacks
//static void...

int main(int argc, char** argv) {
    if (!glfwInit()) { std::cerr << "Failed to init GLFW\n"; return 1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "PC3D", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create window (OpenGL 3.3 required)\n"; glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    printHelp();


    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        prev = now;
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
