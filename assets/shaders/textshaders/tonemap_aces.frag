#version 330
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D hdrTex;

vec3 ACESFilm(vec3 x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 hdrColor = texture(hdrTex, texCoord).rgb;
    vec3 ldrColor = ACESFilm(hdrColor);
    FragColor = vec4(ldrColor, 1.0);
}

