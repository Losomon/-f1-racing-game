#version 330
precision mediump float;

uniform sampler2D hdrTex;
uniform float threshold = 0.9;
uniform float knee = 0.1;

in vec2 texCoord;
out vec4 FragColor;

float luminance(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
    vec3 hdr = texture(hdrTex, texCoord).rgb;
    float lum = luminance(hdr);
    
    float bloom = smoothstep(threshold - knee, threshold, lum);
    FragColor = vec4(hdr * bloom, 1.0);
}
