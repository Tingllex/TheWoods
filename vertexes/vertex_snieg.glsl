#version 330 core
// Dane z VAO
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;
layout( location = 2 ) in vec3 inNormal;
layout( location = 3 ) in mat4 matModelInst;
// Dane do kolejnego etapu
out VS_OUT
{
	vec3 fragPos;
	vec2 fragUV;
	vec3 fragNormal;
};

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

void main()
{
	fragPos = inPosition.xyz;
	fragUV = inUV;
	fragNormal = inNormal;

	gl_Position = matProj * matView * matModelInst * inPosition;
}
