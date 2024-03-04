#ifndef CPROGRAM_H
#define CPROGRAM_H
#include <iostream>
#include "CLight.h"

class CProgram {
public:
    GLuint idProgram;

    CProgram() {
        idProgram = 0;
    }

    GLuint GetId() const {
        return idProgram;
    }

    void CreateProgram() {
        idProgram = glCreateProgram();
    }

    void LoadShaders(const char* vertexFile, const char* fragmentFile) {
        glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, vertexFile));
        glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, fragmentFile));
        LinkAndValidateProgram(idProgram);
    }

    void SetMatrices(const glm::mat4& matProj, const glm::mat4& matView) {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
        glUniformMatrix4fv(glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
    }
    void SetMatrices(const glm::mat4& matProj, const glm::mat4& matView, const glm::mat4& matModel) {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
        glUniformMatrix4fv(glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
        glUniformMatrix4fv(glGetUniformLocation(idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
    }

    void LinkProgram() {
        glLinkProgram(idProgram);

        // Sprawdzenie, czy linkowanie siê powiod³o
        GLint linkSuccess;
        glGetProgramiv(idProgram, GL_LINK_STATUS, &linkSuccess);
        if (!linkSuccess) {
            GLchar messages[256];
            glGetProgramInfoLog(idProgram, sizeof(messages), 0, &messages[0]);
            std::cout << "Linking program failed: " << messages << std::endl;
            exit(1);
        }
    }
    void SetUniform(const std::string& name, const glm::mat4& matrix) {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void SetUniform(const std::string& name, const glm::vec3& vector) {
        glUniform3fv(glGetUniformLocation(idProgram, name.c_str()), 1, glm::value_ptr(vector));
    }

    void SetUniform(const std::string& name, const glm::mat3& matrix) {
        glUniformMatrix4fv(glGetUniformLocation(idProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void SetUniform(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(idProgram, name.c_str()), value);
    }

    void SetUniform(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(idProgram, name.c_str()), value);
    }

    void SetCamera(const glm::vec3& cameraPos) {
        glUniform3fv(glGetUniformLocation(idProgram, "cameraPos"), 1, &cameraPos[0] );
    }

    void SetTime(const std::string& name, float value ) {
        glUniform1f(glGetUniformLocation(idProgram, name.c_str()), value);
    }

    void SetLightParameters(const LightParam& light) {
        glUniform3fv(glGetUniformLocation(idProgram, "myLight.Ambient"), 1, glm::value_ptr(light.Ambient));
        glUniform3fv(glGetUniformLocation(idProgram, "myLight.Diffuse"), 1, glm::value_ptr(light.Diffuse));
        glUniform3fv(glGetUniformLocation(idProgram, "myLight.Specular"), 1, glm::value_ptr(light.Specular));
        glUniform3fv(glGetUniformLocation(idProgram, "myLight.Attenuation"), 1, glm::value_ptr(light.Attenuation));
        glUniform3fv(glGetUniformLocation(idProgram, "myLight.Position"), 1, glm::value_ptr(light.Position));
    }

    void UseProgram() {
        glUseProgram(idProgram);
    }

    void Delete() {
        glDeleteProgram(idProgram);
    }
};

#endif // CPROGRAM_H
