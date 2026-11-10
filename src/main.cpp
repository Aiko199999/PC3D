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
    std::cout << "\nPC3D controls/\n"
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

static void buildTour(CameraTour& t) {
    t.keys = {
        {{ 9.0f, 3.5f,  9.0f}, {0, 2.4f, 0}},   // 3/4 exterior view
        {{ 0.0f, 3.0f, 12.0f}, {0, 2.4f, 0}},   // front
        {{-9.0f, 4.0f,  7.0f}, {0, 2.4f, 0}},   // left side
        {{-8.0f, 6.0f, -8.0f}, {0, 2.8f, 0}},   // back, high
        {{ 8.0f, 3.0f, -6.0f}, {0, 2.4f, 0}},   // back right
        {{ 7.0f, 2.2f,  3.0f}, {-0.8f, 2.3f, 0}}, // glass side, close to the GPU/CPU
        {{ 4.5f, 2.6f,  2.0f}, {-0.8f, 2.3f, 0}}, // moves "inside" through exploded glass
        {{ 9.0f, 5.5f,  9.0f}, {0, 2.4f, 0}},   // pull back
    };
    t.segmentDuration = 3.2f;
}

//callbacks
static void onResize(GLFWwindow* w, int width, int height) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(w));
    app->fbW = std::max(width, 1); app->fbH = std::max(height, 1);
    glViewport(0, 0, width, height);
}
static void onMouseButton(GLFWwindow* w, int button, int action, int) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(w));
    bool down = action == GLFW_PRESS;
    if (button == GLFW_MOUSE_BUTTON_LEFT) app->leftDown = down;
    if (button == GLFW_MOUSE_BUTTON_RIGHT) app->rightDown = down;
    glfwGetCursorPos(w, &app->lastX, &app->lastY);
}
static void onCursor(GLFWwindow* w, double x, double y) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(w));
    float dx = static_cast<float>(x - app->lastX), dy = static_cast<float>(y - app->lastY);
    app->lastX = x; app->lastY = y;
    if (app->tour.active) return;
    if (app->leftDown) app->orbit.rotate(dx, dy);
    if (app->rightDown) app->orbit.pan(dx, dy);
}
static void onScroll(GLFWwindow* w, double, double yoff) {
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(w));
    if (!app->tour.active) app->orbit.zoom(static_cast<float>(yoff));
}
static void onKey(GLFWwindow* w, int key, int, int action, int mods) {
    if (action != GLFW_PRESS) return;
    auto* app = static_cast<App*>(glfwGetWindowUserPointer(w));
    switch (key) {
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(w, GLFW_TRUE); break;
    case GLFW_KEY_E: app->explodeTarget = !app->explodeTarget; break;
    case GLFW_KEY_L: app->lightOrbit = !app->lightOrbit; break;
    case GLFW_KEY_R: app->orbit = OrbitCamera(); app->tour.stop(); break;
    case GLFW_KEY_T:
        if (app->tour.active) app->tour.stop();
        else { app->tour.start(); app->selected = -1; }
        break;
    case GLFW_KEY_N: {
        int n = static_cast<int>(app->scene.inspectable.size());
        if (n == 0) break;
        if (mods & GLFW_MOD_SHIFT) app->selected = (app->selected <= 0) ? n - 1 : app->selected - 1;
        else app->selected = (app->selected + 1) % n;
        std::cout << "Selected: " << app->scene.inspectable[app->selected]->name << "\n";
        break;
    }
    default: break;
    }
}

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

    {
        std::string shaderDir = findShaderDir(argv[0]);
        Shader shader(shaderDir + "/phong.vert", shaderDir + "/phong.frag");

        std::unique_ptr<Mesh> box(Mesh::makeBox());
        std::unique_ptr<Mesh> cyl(Mesh::makeCylinder(32));
        pcmodel::build(app.scene, box.get(), cyl.get());
        buildTour(app.tour);
        if (shotYaw > -900.0f) app.orbit.yaw = glm::radians(shotYaw);
        if (shotExplode) { app.explodeTarget = true; app.scene.explodeFactor = 1.0f; }

        printHelp();

        double prev = glfwGetTime();
        double titleTimer = 0.0;
        int frames = 0;
        int shotFrames = 0;

        while (!glfwWindowShouldClose(window)) {
            double now = glfwGetTime();
            float dt = static_cast<float>(now - prev);
            prev = now;
            glfwPollEvents();

            //animation state
            if (app.tour.active) {
                CameraPose pose;
                if (app.tour.update(dt, pose)) {
                    float t = app.tour.time;
                    app.explodeTarget = (t > 9.0f && t < 20.0f);
                    app.orbit.setFromPose(pose);
                }
                else {
                    app.explodeTarget = false;
                }
            }
            float step = dt / 2.8f;
            if (shotPath.empty()) {
                if (app.explodeTarget) app.scene.explodeFactor = std::min(1.0f, app.scene.explodeFactor + step);
                else                   app.scene.explodeFactor = std::max(0.0f, app.scene.explodeFactor - step);
            }
            app.scene.update(dt);

            //render
            glClearColor(0.05f, 0.06f, 0.10f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            float aspect = static_cast<float>(app.fbW) / static_cast<float>(app.fbH);
            glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 200.0f);
            glm::vec3 camPos = app.orbit.position();
            glm::mat4 view = glm::lookAt(camPos, app.orbit.target, glm::vec3(0, 1, 0));

            float lt = app.lightOrbit ? static_cast<float>(now) * 0.4f : 0.8f;
            glm::vec3 lightPos(7.0f * std::cos(lt), 7.5f, 7.0f * std::sin(lt));

            shader.use();
            shader.setMat4("uView", view);
            shader.setMat4("uProj", proj);
            shader.setVec3("uViewPos", camPos);
            shader.setVec3("uLightPos", lightPos);
            shader.setVec3("uLightColor", glm::vec3(1.25f, 1.2f, 1.1f));

            const Node* hl = (app.selected >= 0) ? app.scene.inspectable[app.selected] : nullptr;
            float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(now) * 4.0f);
            app.scene.draw(shader, hl, pulse);

            if (!shotPath.empty() && ++shotFrames == 3) {
                saveScreenshot(shotPath, app.fbW, app.fbH);
                break;
            }

            glfwSwapBuffers(window);

            //FPS in the title
            ++frames; titleTimer += dt;
            if (titleTimer >= 0.5) {
                char buf[160];
                std::snprintf(buf, sizeof(buf), "PC3D | %.0f FPS | explode %.0f%%%s%s", frames / titleTimer,
                    app.scene.explodeFactor * 100.0f, app.tour.active ? " | TOUR" : "",
                    hl ? (" | " + hl->name).c_str() : "");
                glfwSetWindowTitle(window, buf);
                frames = 0; titleTimer = 0.0;
            }
        }

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}