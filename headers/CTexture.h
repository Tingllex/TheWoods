#ifndef CTEXTURE_H
#define CTEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class CTexture {
public:
    int tex_width, tex_height, tex_n;
    unsigned char* tex_data;
    GLuint idTexture;
    bool textureLoaded;


    CTexture() {
        textureLoaded = false;
    }
    void LoadTexture(const char* texture_filename)
    {
        tex_data = stbi_load(texture_filename, &tex_width, &tex_height, &tex_n, 0);
        if ( tex_data == NULL )
        {
            printf("Image can't be loaded!\n");
        }
        else
        {
            textureLoaded = true;
        }
    }

    void CreateTextureObject()
    {
        glGenTextures(1, &idTexture);
        glBindTexture(GL_TEXTURE_2D, idTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    void ActiveTexture(GLuint idProgram)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, idTexture);
        glUniform1i(glGetUniformLocation(idProgram, "tex"), 0);
        glUniform1i(glGetUniformLocation(idProgram, "texActive"), 1);
    }

    void DisableTexture(GLuint idProgram)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glUniform1i(glGetUniformLocation(idProgram, "texActive"), 0);
    }

    bool GetTextureLoaded()
    {
        return textureLoaded;
    }
};


#endif // CTEXTURE_H
