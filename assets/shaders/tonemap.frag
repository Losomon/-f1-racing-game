#version 330
precision mediump float;

uniform sampler2D hdrTex;
uniform float exposure = 1.0;

in vec2 texCoord;
out vec4 FragColor;

vec3 ACESFilmic(vec3 x) {
    const float a = 2.51f;
    const float b = 0.03f;
    const float c = 2.43f;
    const float d = 0.59f;
    const float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec3 hdr = texture(hdrTex, texCoord).rgb * exposure;
    vec3 ldr = ACESFilmic(hdr);
    FragColor = vec4(ldr, 1.0);
}
