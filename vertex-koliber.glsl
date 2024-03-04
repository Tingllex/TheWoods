#version 330 core

// Atrybuty wierzcholkow z VAO
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;
layout( location = 3 ) in mat4 matModelInst;

// Macierz rzutowania i transformacji
uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

uniform vec3 cameraPos;

// Dane wyjsciowe do shadera fragmentow
out vec4 fragPos;
out vec2 fragUV;
out vec3 fragNormal;
//out vec3 lightCoef;

uniform mat3 matNormal;

void main()
{
    float angle = radians(-85.0);
    mat4 rotationMatrix = mat4(
        cos(angle), 0, sin(angle), 0,
        0, 1, 0, 0,
        -sin(angle), 0, cos(angle), 0,
        0, 0, 0, 1
    );

	// Przekazanie danych do shadera fragmentow
	fragPos = matModel * inPosition; //v
    fragUV = inUV;
    fragNormal = matNormal * inNormal; //n

    gl_Position = matProj * matView *  (matModelInst * rotationMatrix) * inPosition;
}
