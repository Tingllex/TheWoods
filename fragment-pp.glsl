#version 150 core

// Kolor ostateczny
out vec4 outColor;

// Dane wejsciowe z shadera wierzcholkow
in vec4 fragPos;
in vec2 fragUV;

uniform sampler2D tex;
uniform bool effect1;
uniform bool effect2;
uniform bool effect3; // Add a new uniform for the convolution effect
uniform float kernel[9]; // Convolution kernel

void main()
{
    if (effect1) {
        vec4 texColor = texture(tex, fragUV);
        float average = (texColor.r + texColor.g + texColor.b) / 3.0;

        if (average < 0.5) {
            outColor = vec4(average * 2.0, 0.0, 0.0, texColor.a);
        } else {
            outColor = vec4(1.0, (average - 0.5) * 2.0, 0.0, texColor.a);
        }
    } else if (effect2) {
        vec2 uv = fragUV;
        float pixelSize = 0.05;
        uv = floor(uv / pixelSize) * pixelSize;
        outColor = texture(tex, uv);
    } else if (effect3) {
        vec2 tex_offset = 1.0 / textureSize(tex, 0); // gets the size of one texel
        vec3 result = vec3(0.0);

        for(int i = -1; i <= 1; i++) {
            for(int j = -1; j <= 1; j++) {
                vec2 offset = vec2(tex_offset.x * j, tex_offset.y * i);
                result += texture(tex, fragUV + offset).rgb * kernel[(i+1)*3 + j+1];
            }
        }

        outColor = vec4(result, 1.0);
    } else {
        outColor = texture(tex, fragUV);
    }
}
