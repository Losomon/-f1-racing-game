#version 330
precision mediump float;

uniform sampler2D colorTex;
uniform float strength = 0.003;
uniform float pulse = 0.0;

in vec2 texCoord;
out vec4 FragColor;

void main() {
    vec2 dir = texCoord - 0.5;
    vec3 col;
    col.r = texture(colorTex, texCoord + dir * strength * (0.5 + pulse)).r;
    col.g = texture(colorTex, texCoord).g;
    col.b = texture(colorTex, texCoord - dir * strength * (0.5 + pulse)).b;
    FragColor = vec4(col, 1.0);
}
