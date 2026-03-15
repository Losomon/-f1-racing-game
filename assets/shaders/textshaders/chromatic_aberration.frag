#version 330
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform float strength = 0.005;

void main() {
    vec2 uv = texCoord;
    vec3 color = vec3(
        texture(tex, uv + vec2(-strength, 0)).r,
        texture(tex, uv).g,
        texture(tex, uv + vec2(strength, 0)).b
    );
    FragColor = vec4(color, 1.0);
}

