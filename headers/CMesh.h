#ifndef CMESH_H
#define CMESH_H
#define NUM_KOLIBER_INSTANCES 50
#include "CTexture.h"
#include "collider.hpp"

class CMesh {
public:
    GLuint idProgram;
    GLuint idVAO;
    GLuint idVBO_coord;
    GLuint idVBO_uv;
    GLuint idVBO_normal;
    GLuint vInstances;
    CTexture texture;
    glm::mat4x4 matModel;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    CMesh() {
//        idVAO = 0;
//        idVBO_coord = 0;
//        idVBO_uv = 0;
//        idVBO_normal = 0;
    }


    void SetMatModel(const glm::mat4& matModel){
        this->matModel = matModel;
        GLint idProgram = 0;
        glGetIntegerv(GL_CURRENT_PROGRAM, &idProgram);
        glm::mat3 matNormal = glm::mat3(transpose(inverse(matModel)));
        glUniformMatrix4fv( glGetUniformLocation(idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel) );
        glUniformMatrix3fv( glGetUniformLocation(idProgram, "matNormal"), 1, GL_FALSE, glm::value_ptr(matNormal) );
    }


    void CreateFromOBJ(GLuint idProgram, const char* filename, const char* texname) {
        this->idProgram = idProgram;
        if (!loadOBJ(filename, vertices, uvs, normals))
        {
            printf("File not loaded!\n");
        }
        printf("Loaded %d vertices\n", GetVertexCount());

        glGenVertexArrays( 1, &idVAO );
        glBindVertexArray( idVAO );

        // Bufor na wspolrzedne wierzcholkow
        glGenBuffers( 1, &idVBO_coord );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
        glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

        glGenBuffers(1, &idVBO_uv);
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_uv );
        glBufferData( GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW );
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &idVBO_normal);
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_normal );
        glBufferData( GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW );
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);


        glBindVertexArray( 0 );

        if (texname != NULL)
        {
            this->texture.LoadTexture(texname);
            this->texture.CreateTextureObject();
        }
    }
    void CreateFromOBJMultiple(glm::mat4 matModels[], GLuint idProgram, const char* filename, const char* texname) {
        this->idProgram = idProgram;
        if (!loadOBJ(filename, vertices, uvs, normals))
        {
            printf("File not loaded!\n");
        }
        printf("Loaded %d vertices\n", GetVertexCount());

        glGenVertexArrays( 1, &idVAO );
        glBindVertexArray( idVAO );

        // Bufor na wspolrzedne wierzcholkow
        glGenBuffers( 1, &idVBO_coord );
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_coord );
        glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW );
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
        glEnableVertexAttribArray( 0 );

        glGenBuffers(1, &idVBO_uv);
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_uv );
        glBufferData( GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW );
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &idVBO_normal);
        glBindBuffer( GL_ARRAY_BUFFER, idVBO_normal );
        glBufferData( GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW );
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);

        if (texname != NULL)
        {
            this->texture.LoadTexture(texname);
            this->texture.CreateTextureObject();
        }

        glGenBuffers (1, &vInstances);
        glBindBuffer (GL_ARRAY_BUFFER, vInstances);
        glBufferData (GL_ARRAY_BUFFER, NUM_KOLIBER_INSTANCES * sizeof (glm :: mat4), &matModels[0], GL_STATIC_DRAW);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void *)0);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void *)(sizeof(glm::vec4)));
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void *)(2 * sizeof(glm::vec4)));
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof (glm::mat4), (void *)(3 * sizeof(glm::vec4)));
        glEnableVertexAttribArray (3);
        glEnableVertexAttribArray (4);
        glEnableVertexAttribArray (5);
        glEnableVertexAttribArray (6);
        glVertexAttribDivisor (3, 1);
        glVertexAttribDivisor (4, 1);
        glVertexAttribDivisor (5, 1);
        glVertexAttribDivisor (6, 1);

        glBindVertexArray( 0 );
    }

    void Draw() {
        if (this->texture.GetTextureLoaded())
        {
            this->texture.ActiveTexture(this->idProgram);
        }
        else {
            this->texture.DisableTexture(this->idProgram);
        }
        glBindVertexArray(idVAO);
        glDrawArrays(GL_TRIANGLES, 0, GetVertexCount());
        glBindVertexArray(0);
    }

    void DrawInstances() {
        if (this->texture.GetTextureLoaded())
        {
            this->texture.ActiveTexture(this->idProgram);
        }
        else {
            this->texture.DisableTexture(this->idProgram);
        }
        glBindVertexArray(idVAO);
        glDrawArraysInstanced( GL_TRIANGLES, 0, vertices.size(), 100 );
        glBindVertexArray(0);
    }

    int GetVertexCount() {
        return vertices.size();
    }

    std::vector<glm::vec3> GetVertices() {
        return vertices;
    }

    int GetvInstances() {
        return vInstances;
    }

    void Delete() {
        glDeleteVertexArrays( 1, &idVBO_coord );
        glDeleteVertexArrays( 1, &idVBO_uv );
        glDeleteVertexArrays( 1, &idVBO_normal );
        glDeleteVertexArrays( 1, &idVAO );
    }
};

class CSceneObject : public CMesh
{
public:

	// Dodatkowe skladowe wzgledem CMesh

	// ------------------------------------------
	// NOWE: Collider do testu kolizji
	// ------------------------------------------
	CCollider *Collider = NULL;

	glm::vec3 Position;
	// ------------------------------------------
	// NOWE: Ustawienie pozycji obiektu na scenie
	// ------------------------------------------
	void SetPosition(glm::vec3 _pos)
	{
		this->Position = _pos;
		matModel = glm::translate(glm::mat4(1.0), this->Position);
	}


};
#endif // CMESH_H
