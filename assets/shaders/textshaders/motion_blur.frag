#version 330
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D sceneTex;
uniform sampler2D velocityTex;
uniform float blurAmount = 0.01;

void main() {
    vec2 velocity = texture(velocityTex, texCoord).rg * blurAmount;
    vec3 color = vec3(0.0);
    int samples = 16;
    for (int i = -8; i <= 8; ++i) {
        vec2 offset = velocity * float(i) / 8.0;
        color += texture(sceneTex, texCoord + offset).rgb;
    }
    color /= float(samples);
    FragColor = vec4(color, 1.0);
}

