#version 150 core

// Dane z vertex shadera
in VS_OUT
{
	vec3 fragPos;
	vec2 fragUV;
	vec3 fragNormal;
};

// Kolor ostateczny
out vec4 outColor;

// Zmienne jednorodne
uniform vec3 cameraPos;
uniform sampler2D tex0;
uniform samplerCube tex_skybox;

void main()
{
	vec3 fragColor = texture(tex0, fragUV).rgb;
    vec3 D = normalize(fragPos - cameraPos);
    vec3 Refl = reflect (D, fragNormal);
    vec4 ReflColor = texture ( tex_skybox, Refl);
    vec4 finalColor = ReflColor * vec4(fragColor, 1.0)/0.5;
	outColor = finalColor;
}

