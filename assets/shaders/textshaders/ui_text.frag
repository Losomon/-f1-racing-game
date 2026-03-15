#version 330
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D fontTex;
uniform vec4 textColor = vec4(1.0);

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    float dist = median(texture(fontTex, texCoord).r, texture(fontTex, texCoord).g, texture(fontTex, texCoord).b) - 0.5;
    float alpha = clamp(dist / fwidth(dist) + 0.5, 0.0, 1.0);
    FragColor = vec4(textColor.rgb, alpha);
}

