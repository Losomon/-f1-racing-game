#version 330
in vec3 texCoord;
out vec4 FragColor;

uniform samplerCube envMap;

void main() {
    vec3 color = texture(envMap, texCoord).rgb;
    FragColor = vec4(color, 1.0);
}

