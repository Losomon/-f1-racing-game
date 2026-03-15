#version 330
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D hdrTex;
uniform float threshold = 1.0;

void main() {
    vec3 hdrColor = texture(hdrTex, texCoord).rgb;
    float brightness = dot(hdrColor, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > threshold) {
        FragColor = vec4(hdrColor, 1.0);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

