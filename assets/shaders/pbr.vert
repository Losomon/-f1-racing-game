#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform bool computeTBN = true;

out vec2 texCoord;
out vec3 worldPos;
out mat3 TBNMat;
out vec3 fragNormal;

void main() {
    vec4 worldPosition = model * vec4(position, 1.0);
    gl_Position = proj * view * worldPosition;
    
    texCoord = uv;
    worldPos = worldPosition.xyz;
    
    vec3 N = normalize((model * vec4(normal, 0.0)).xyz);
    fragNormal = N;
    
    if (computeTBN) {
        vec3 T = normalize((model * vec4(tangent, 0.0)).xyz);
        vec3 B = normalize((model * vec4(bitangent, 0.0)).xyz);
        TBNMat = mat3(T, B, N);
    }
}
