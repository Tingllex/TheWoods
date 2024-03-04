#version 150 core

// Kolor ostateczny
out vec4 outColor;

// Dane wejsciowe z shadera wierzcholkow
in vec4 fragPos;
in vec2 fragUV;

uniform sampler2D tex;
uniform bool effect1;
uniform bool effect2;

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
    }
    if (effect2) {
        vec2 uv = fragUV;
        float pixelSize = 0.05;
        uv = floor(uv / pixelSize) * pixelSize;
        outColor = texture( tex, uv );
    }
    if (!effect1 && !effect2) {
        outColor = texture( tex, fragUV );
    }
}
