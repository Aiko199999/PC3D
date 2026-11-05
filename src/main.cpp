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
#include "pc_model.h"
#include "scene.h"
#include "shader.h"

namespace fs = std::filesystem;

struct App {
    OrbitCamera orbit;
    CameraTour tour;
    Scene scene;
    bool explodeTarget = false;
    bool lightOrbit = true;
    int selected = -1;
    double lastX = 0, lastY = 0;
    bool leftDown = false, rightDown = false;
    int fbW = 1280, fbH = 720;
};

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

static std::string findShaderDir(const char* argv0) {
    std::vector<fs::path> candidates;
    try { candidates.push_back(fs::absolute(argv0).parent_path() / "shaders"); }
    catch (...) {}
    candidates.push_back(fs::current_path() / "shaders");
#ifdef PC3D_SHADER_SRC_DIR
    candidates.push_back(PC3D_SHADER_SRC_DIR);
#endif
    for (auto& c : candidates)
        if (fs::exists(c / "phong.vert")) return c.string();
    std::cerr << "[PC3D] Could not find the shaders/ folder!\n";
    return "shaders";
}

static void saveScreenshot(const std::string& path, int w, int h) {
    std::vector<unsigned char> px(static_cast<size_t>(w) * h * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, px.data());
    FILE* f = std::fopen(path.c_str(), "wb");
    if (!f) { std::cerr << "Cannot write " << path << "\n"; return; }
    std::fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int y = h - 1; y >= 0; --y) std::fwrite(&px[static_cast<size_t>(y) * w * 3], 1, static_cast<size_t>(w) * 3, f);
    std::fclose(f);
    std::cout << "Saved " << path << "\n";
}

//callbacks
//static void...

int main(int argc, char** argv) {
    std::string shotPath;
    bool shotExplode = false;
    float shotYaw = -999.0f;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--shot" && i + 1 < argc) shotPath = argv[++i];
        else if (a == "--explode") shotExplode = true;
        else if (a == "--yaw" && i + 1 < argc) shotYaw = std::stof(argv[++i]);
    }

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

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to load OpenGL functions\n"; return 1;
    }
    std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\nGPU:    " << glGetString(GL_RENDERER) << "\n";

    App app;
    glfwSetWindowUserPointer(window, &app);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onCursor);
    glfwSetScrollCallback(window, onScroll);
    glfwSetKeyCallback(window, onKey);
    glfwGetFramebufferSize(window, &app.fbW, &app.fbH);
    glViewport(0, 0, app.fbW, app.fbH);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);

    //GL objects...

    printHelp();

    double prev = glfwGetTime();
    double titleTimer = 0.0;
    int frames = 0;
    int shotFrames = 0;

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        prev = now;
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
