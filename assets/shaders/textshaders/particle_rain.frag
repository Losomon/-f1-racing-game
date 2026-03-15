#version 330
in float vLifetime;
out vec4 FragColor;

uniform float time;

void main() {
    float alpha = vLifetime * (1.0 - length(gl_PointCoord - 0.5) * 2.0);
    vec3 color = vec3(0.5, 0.7, 1.0) * alpha;
    FragColor = vec4(color, alpha);
}

