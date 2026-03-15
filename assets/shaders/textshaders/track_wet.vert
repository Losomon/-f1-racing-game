#version 330
// Same as pbr.vert but extra ripple UV
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec3 cameraPos;
uniform float time;

out vec2 texCoord;
out vec2 rippleUV;
out vec3 worldPos;
out mat3 TBN;

void main() {
    texCoord = uv;
    rippleUV = uv * 3.0 + vec2(time * 1.2, 0.0);
    worldPos = (model * vec4(position, 1.0)).xyz;
    vec3 T = normalize((model * vec4(tangent, 0.0)).xyz);
    vec3 B = normalize((model * vec4(bitangent, 0.0)).xyz);
    vec3 N = normalize((model * vec4(normal, 0.0)).xyz);
    TBN = mat3(T, B, N);
    gl_Position = proj * view * model * vec4(position, 1.0);
}

