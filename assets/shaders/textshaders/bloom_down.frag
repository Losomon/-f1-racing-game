#version 330
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform vec2 texelSize;

void main() {
    vec3 color = vec3(0.0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            color += texture(tex, texCoord + vec2(x, y) * texelSize).rgb;
        }
    }
    FragColor = vec4(color / 9.0, 1.0);
}

