#version 330
layout(location = 0) in vec2 position;
layout(location = 1) in float lifetime;

uniform mat4 proj;
uniform mat4 view;
uniform float time;

out float vLifetime;

void main() {
    vLifetime = lifetime;
    vec2 pos = position + vec2(sin(time * 5.0 + position.x * 10.0) * 0.1, time * 10.0);
    gl_Position = proj * view * vec4(pos, 0.0, 1.0);
    gl_PointSize = 2.0 + lifetime * 3.0;
}

