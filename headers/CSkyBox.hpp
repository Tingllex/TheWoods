#ifndef CSKYBOX_HPP
#define CSKYBOX_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

class CSkyBox {
public:
    CSkyBox();
    ~CSkyBox();
    void Initialize(int skyboxIndex);
    void Render(const glm::mat4& matProj, const glm::mat4& matView);
    GLuint GetTextureId() const { return Texture; }
private:
    GLuint VAO;
    GLuint Program;
    GLuint Texture;
    const char** files;
    static const char* skybox1_files[];
    static const char* skybox2_files[];
    static const char** skybox_paths[];
    void loadShaders();
    void setupVAO();
    void loadTexture(int skyboxIndex);
};

const char* CSkyBox::skybox1_files[] = {
    "skybox/skybox1/posx.jpg",
    "skybox/skybox1/negx.jpg",
    "skybox/skybox1/posy.jpg",
    "skybox/skybox1/negy.jpg",
    "skybox/skybox1/posz.jpg",
    "skybox/skybox1/negz.jpg",
};

const char* CSkyBox::skybox2_files[] = {
    "skybox/skybox2/posx.jpg",
    "skybox/skybox2/negx.jpg",
    "skybox/skybox2/posy.jpg",
    "skybox/skybox2/negy.jpg",
    "skybox/skybox2/posz.jpg",
    "skybox/skybox2/negz.jpg",
};

const char** CSkyBox::skybox_paths[] = { CSkyBox::skybox1_files, CSkyBox::skybox2_files };

CSkyBox::CSkyBox() : VAO(0), Program(0), Texture(0), files(nullptr) {}

CSkyBox::~CSkyBox() {
    // Cleanup
    glDeleteProgram(Program);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VAO);
}

void CSkyBox::Initialize(int skyboxIndex) {
    if (skyboxIndex < 0 || skyboxIndex >= 2) {
        throw std::runtime_error("Invalid skybox index");
    }

    files = skybox_paths[skyboxIndex];
    loadShaders();
    setupVAO();
    loadTexture(skyboxIndex);
}

void CSkyBox::Render(const glm::mat4& matProj, const glm::mat4& matView) {
    glUseProgram(Program);

    glm::mat4 matPVM = matProj * matView * glm::scale(glm::mat4(1), glm::vec3(100.0, 100.0, 100.0));
    glUniformMatrix4fv(glGetUniformLocation(Program, "matPVM"), 1, GL_FALSE, glm::value_ptr(matPVM));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);
    glUniform1i(glGetUniformLocation(Program, "tex_skybox"), 0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);

    glUseProgram(0);
}

void CSkyBox::loadShaders() {
    Program = glCreateProgram();
    glAttachShader(Program, LoadShader(GL_VERTEX_SHADER, "vertexes/skybox-vertex.glsl"));
    glAttachShader(Program, LoadShader(GL_FRAGMENT_SHADER, "fragments/skybox-fragment.glsl"));
    LinkAndValidateProgram(Program);
}

void CSkyBox::setupVAO() {
    GLfloat positions[8 * 3] = {
        1.0f, 1.0f, 1.0f,   // 0
        -1.0f, 1.0f, 1.0f,  // 1
        -1.0f, -1.0f, 1.0f, // 2
        1.0f, -1.0f, 1.0f,  // 3
        1.0f, 1.0f, -1.0f,  // 4
        -1.0f, 1.0f, -1.0f, // 5
        -1.0f, -1.0f, -1.0f,// 6
        1.0f, -1.0f, -1.0f  // 7
    };

    GLuint indices[12 * 3] = {
        5, 0, 1, 5, 4, 0, 2, 0, 3, 2, 1, 0,
        7, 0, 4, 7, 3, 0, 3, 6, 2, 3, 7, 6,
        1, 2, 6, 1, 6, 5, 4, 5, 6, 4, 6, 7
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint vBuffer_pos;
    glGenBuffers(1, &vBuffer_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);

    GLuint vBuffer_idx;
    glGenBuffers(1, &vBuffer_idx);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vBuffer_idx);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void CSkyBox::loadTexture(int skyboxIndex) {
    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, Texture);

    stbi_set_flip_vertically_on_load(false);

    const GLenum targets[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    for (int i = 0; i < 6; ++i) {
        int tex_width, tex_height, n;
        unsigned char* tex_data = stbi_load(files[i], &tex_width, &tex_height, &n, 0);
        if (tex_data == NULL) {
            printf("Image %s can't be loaded!\n", files[i]);
            exit(1);
        }
        glTexImage2D(targets[i], 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        stbi_image_free(tex_data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    stbi_set_flip_vertically_on_load(true);
}
#endif // CSKYBOX_HPP
