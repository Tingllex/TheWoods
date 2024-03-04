#ifndef CLIGHT_H
#define CLIGHT_H

using glm::vec3;
//struktura parametrow swiatla
struct LightParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position; // Direction dla kierunkowego
};

// Przykladowe swiatlo punktowe
struct LightParam myLight[3] =
{
	{
	    vec3 (0.1 , 0.1 , 0.1), // ambient
        vec3 (1.0 , 1.0 , 1.0), // diffuse
        vec3 (1.0 , 1.0 , 1.0), // specular
        vec3 (1.0 , 0.0 , 0.01), // attenuation
        vec3 (2.0 , 3.0 , 1.0)  // position
	},
	{
	    vec3 (0.1 , 0.1 , 0.1), // ambient
        vec3 (1.0 , 1.0 , 1.0), // diffuse
        vec3 (1.0 , 1.0 , 1.0), // specular
        vec3 (1.0 , 0.0 , 0.1), // attenuation
        vec3 (3.0 , 5.0 , 1.0)  // position
	},
	{
	    vec3(0.1, 0.1, 0.1),    // ambient
        vec3(1.0, 1.0, 1.0),    // diffuse
        vec3(3.0, 3.0, 3.0),    // specular
        vec3(1.0, 0.0, 0.005),    // attenuation
        vec3(3.0, 5.0, 1.0)     // position
	}
};
#endif // CLIGHT_H
