#version 330 core

// Atrybuty wierzcholkow z VAO
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;

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
out vec4 fragPosLight;

uniform mat4 lightProj;
uniform mat4 lightView;
uniform mat3 matNormal;

void main()
{
	// Przekazanie danych do shadera fragmentow
	fragPos = matModel * inPosition; //v
    fragUV = inUV;
    fragNormal = matNormal * inNormal; //n

    // Wspolrzedne wierzcholka w bryle obcinania kamery oswietlenia
	fragPosLight = lightProj * lightView * matModel * inPosition;

    gl_Position = matProj * matView * matModel * inPosition;
}
