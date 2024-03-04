#version 150 core

// Kolor ostateczny
out vec4 outColor;

// Dane wejsciowe z shadera wierzcholkow
in vec4 fragPos;
in vec2 fragUV;
in vec3 fragNormal;

//wspolczynnik swiatla
vec3 lightCoef;

// Pozycja kamery przekazana z aplikacji
uniform vec3 cameraPos;
uniform sampler2D tex;
uniform samplerCube tex_skybox;
//struktura parametrow swiatla
struct LightParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position; // Direction dla kierunkowego
};

uniform LightParam myLight;

// Struktura materialu obiektu
struct MaterialParam
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

uniform MaterialParam myMaterial;
uniform int lightingModel;
uniform bool uIsMirrorEffect;
uniform bool uIsTransparencyEffect;
uniform float Time;

//funkcja liczaca oswietlenie punktowe
vec3 calculatePointLight(LightParam light, MaterialParam material, int shadingModel)
{
    vec3 lightCoef = vec3(0.3);

    vec3 AmbientPart;
    AmbientPart = light.Ambient * material.Ambient;

    vec3 DiffusePart;
    float diff = 1.0;
    vec3 L = normalize(light.Position - fragPos.xyz);
    diff = max( dot(L, fragNormal), 0);
    DiffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 SpecularPart;
    float spec = 1.0;
    vec3 E = normalize(cameraPos - fragPos.xyz);
//    vec3 R = reflect(-E, inNormal);
//    spec = pow(max(dot(R, L), 0), material.Shininess);

    if (shadingModel == 0 ) // Phong
    {
        vec3 R = reflect(-E, fragNormal);
        spec = pow(max(dot(R, L), 0 ), material.Shininess);
    }
    else if (shadingModel == 1) // Blinn-Phong
    {
        vec3 H = normalize(L + E);
        spec = pow(max(dot(H, fragNormal), 0 ), material.Shininess);
    }

    SpecularPart = spec * light.Specular * material.Specular;

    float LV = distance(fragPos.xyz, light.Position);
    float Latt = 1.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);
    //float Latt = 1.0 / (light.Attenuation.x + light.Attenuation.y * (LV / 2.0) + light.Attenuation.z * (LV / 4.0));

    //glowny wzor na swiatlo
    lightCoef = AmbientPart + Latt* ( DiffusePart + SpecularPart);
    return lightCoef;
}

vec3 calculateDirectionalLight(LightParam light, MaterialParam material, int shadingModel, bool isAnimated)
{
    vec3 lightDir;
    if (isAnimated)
    {
        float rotationSpeed = 5;
        lightDir = normalize(vec3(cos(rotationSpeed * Time), 0.0, sin(rotationSpeed * Time)));
    } else {
        lightDir = normalize(light.Position);
    }


    vec3 AmbientPart = light.Ambient * material.Ambient;

    vec3 DiffusePart;
    float diff = max(dot(lightDir, fragNormal), 0);
    DiffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 SpecularPart;
    float spec = 0.0;

    vec3 E = normalize(cameraPos - fragPos.xyz);

    if (shadingModel == 0) // Phong
    {
        vec3 R = reflect(-lightDir, fragNormal);
        spec = pow(max(dot(R, E), 0), material.Shininess);
    }
    else if (shadingModel == 1) // Blinn-Phong
    {
        vec3 H = normalize(lightDir + E);
        spec = pow(max(dot(H, fragNormal), 0), material.Shininess);
    }

    SpecularPart = spec * light.Specular * material.Specular;

    vec3 lightCoef = AmbientPart + DiffusePart + SpecularPart;
    return lightCoef;
}

void main() {
    vec3 fragColor = texture(tex, fragUV).rgb;

    vec3 viewDir = normalize(fragPos.xyz - cameraPos);
    vec3 reflectedDir = reflect(viewDir, fragNormal);
    vec4 skyboxColor = texture(tex_skybox, reflectedDir);

    switch (lightingModel) {
        case 0:
            lightCoef = calculatePointLight(myLight, myMaterial, 0);
            break;
        case 1:
            lightCoef = calculatePointLight(myLight, myMaterial, 1);
            break;
        case 2:
            outColor = vec4(fragColor, 1.0);
            return;
        case 3:
            outColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        case 4:
            lightCoef = calculateDirectionalLight(myLight, myMaterial, 0, false);
            break;
        case 5:
            lightCoef = calculateDirectionalLight(myLight, myMaterial, 1, false);
            break;
        case 6:
            lightCoef = calculateDirectionalLight(myLight, myMaterial, 0, true);
            break;
        default:
            lightCoef = vec3(1.0);
            break;
    }

    vec4 finalColor = vec4(fragColor * lightCoef, 1.0);

    if (uIsMirrorEffect) {
        vec3 viewDir = normalize(fragPos.xyz - cameraPos);
        vec3 reflectedDir = reflect(viewDir, fragNormal);
        vec4 skyboxColor = texture(tex_skybox, reflectedDir);
        finalColor = mix(finalColor, skyboxColor, 0.5);
    }

    if (uIsTransparencyEffect) {
        finalColor.a = 0.8;
    }

    outColor = finalColor;
}

