#version 330

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec2 inUV;

uniform mat4 matProj;
uniform mat4 matView;

out vec4 fragPos;
out vec2 fragUV;

void main()
{
	gl_Position = matProj * matView * inPosition;

	fragPos = inPosition;
	fragUV = inUV;
}
