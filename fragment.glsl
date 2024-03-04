#version 150 core

// Kolor ostateczny
out vec4 outColor;

// Dane wejsciowe z shadera wierzcholkow
in vec4 fragPos;
in vec2 fragUV;
in vec3 fragNormal;
in vec4 fragPosLight;

//wspolczynnik swiatla
vec3 lightCoef;

// Pozycja kamery przekazana z aplikacji
uniform vec3 cameraPos;
uniform sampler2D tex;
uniform samplerCube tex_skybox;
uniform sampler2D tex_shadowMap;
uniform vec3 lightDirection;
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

// ---------------------------------------------------------------------------
// Zwraca [0-1], gdzie 1 oznacza ze fragment jest calkowicie w cieniu
// ---------------------------------------------------------------------------
float calcDirectionalShadow(vec4 fragPosLight, vec3 fragNormal, vec3 lightDirection)
{

	// Brak cienia
	// return 0;

	// Korekcja perspektywiczna (dla oswietlenia kierunkowego niepotrzebna)
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // przeksztalcenie wartosci [-1,+1] na [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // pobranie z tekstury shadowMap odleglosci od zrodla swiatla fragmentu
    // do fragmentu oswietlonego na drodze do aktualnego fragmentu
    float closestDepth = texture(tex_shadowMap, projCoords.xy).r;

    // obliczenie aktualnej odleglosci od zrodla swiatla
    float currentDepth = projCoords.z;

    // Sprawdzenie czy fragment jest w cieniu po odleglosci
    //	return (currentDepth > closestDepth) ? 1.0 : 0.0;

	// Shadow acne
	// Parametry i stale ustawiamy eksperymentalnie
	float bias = max(0.004 * (1.0 - dot(fragNormal, lightDirection)), 0.001);
	return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}

// ---------------------------------------------------------------------------
// Uproszczone oswietlenie kierunkowe ktore UWAGA: nie zwraca skladowej ambient
// ---------------------------------------------------------------------------
vec3 calcDirectionalLight(vec3 fragPos, vec3 fragNormal, LightParam light)
{

	// Diffuse
	vec3  lightDirection = normalize(light.Position);
	float diffuseCoeff = max(dot(fragNormal, -light.Position), 0.0);
	vec3  resultDiffuse = diffuseCoeff * vec3(light.Diffuse);
	vec3  diffusePart = resultDiffuse;

	// Specular
	vec3  viewDir = normalize(cameraPos - vec3(fragPos));
	vec3  reflectDir = reflect(light.Position, fragNormal);
	float specularCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3  specularPart = specularCoeff * vec3(light.Specular);

	// result
	return (diffusePart + specularPart);
}

void main() {
        LightParam myLight;

	// Parametry swiatla
	myLight.Ambient = vec3(0.2, 0.2, 0.2);
	myLight.Diffuse = vec3(0.8, 0.8, 0.8);
	myLight.Specular = vec3(0.5, 0.5, 0.5);
	myLight.Position = lightDirection;

	// Oswietlenie kierunkowe
	vec3  lightPart = calcDirectionalLight(fragPos.xyz, fragNormal, myLight);
	// Cienie
	float shadowPart = calcDirectionalShadow(fragPosLight, fragNormal, lightDirection);

    vec3 fragColor = texture(tex, fragUV).rgb;

    vec3 viewDir = normalize(fragPos.xyz - cameraPos);
    vec3 reflectedDir = reflect(viewDir, fragNormal);
    vec4 skyboxColor = texture(tex_skybox, reflectedDir);

    vec3 finalColor2 = (myLight.Ambient + (1 - shadowPart) * lightPart) * fragColor;

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

    outColor =  vec4(finalColor2, 1.0);

}

