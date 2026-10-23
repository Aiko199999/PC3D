//GLSL shader compiling helper(Aiym)
#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
public:





private:
    GLint loc(const char* n) const { return glGetUniformLocation(id, n); }

    static std::string readFile(const std::string& path) {
        std::ifstream f(path);
        if (!f) {
            std::cerr << "[Shader] Cannot open file: " << path << std::endl;
            return "";
        }
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    static GLuint compile(GLenum type, const std::string& src, const std::string& name) {
        GLuint s = glCreateShader(type);
        const char* c = src.c_str();
        glShaderSource(s, 1, &c, nullptr);
        glCompileShader(s);
        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetShaderInfoLog(s, 1024, nullptr, log);
            std::cerr << "[Shader] Compile error in " << name << ":\n" << log << std::endl;
        }
        return s;
    }
};
