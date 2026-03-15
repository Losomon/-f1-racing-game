#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 texCoord;

void main() {
    texCoord = uv;
    gl_Position = proj * view * model * vec4(position, 1.0);
}

