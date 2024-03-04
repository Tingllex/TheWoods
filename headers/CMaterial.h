#ifndef CMATERIAL_H
#define CMATERIAL_H

using glm::vec3;

// Struktura materialu obiektu
struct MaterialParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};



// Przykladowy material
struct MaterialParam myMaterial[5] =
{
    { //ball1
        vec3 (0.2 , 0.2 , 0.2), // ambient
        vec3 (1.0 , 1.0 , 1.0), // diffuse
        vec3 (1.5 , 1.5 , 1.5), // specular
        8.0 // shininess
    },
    { // ball2 (shiny)
        vec3(0.2, 0.2, 0.2), // ambient
        vec3(1.0, 1.0, 1.0), // diffuse
        vec3(2.0, 2.0, 2.0), // specular
        16.0 // shininess
    },
    { // ball3 (matte)
        vec3(0.2, 0.2, 0.2), // ambient
        vec3(0.5, 0.5, 0.5), // diffuse
        vec3(0.0, 0.0, 0.0), // specular
        4.0 // shininess
    },
    { //terrain
        vec3 (0.2 , 0.2 , 0.2), // ambient
        vec3 (1.0 , 1.0 , 1.0), // diffuse
        vec3 (1.5 , 1.5 , 1.5), // specular
        8.0 // shininess
    },
    { //koliber
        vec3 (0.2 , 0.2 , 0.2), // ambient
        vec3 (1.0 , 1.0 , 1.0), // diffuse
        vec3 (1.5 , 1.5 , 1.5), // specular
        8.0 // shininess
    }
};
#endif // CMATERIAL_H
