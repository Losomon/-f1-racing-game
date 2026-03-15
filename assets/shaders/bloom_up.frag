#version 330
precision mediump float;

uniform sampler2D lowResTex;  // current downsampled
uniform sampler2D highResTex; // previous higher res
uniform vec2 texelSize;
uniform float intensity = 1.2;

in vec2 texCoord;
out vec4 FragColor;

void main() {
    vec3 low = texture(lowResTex, texCoord).rgb;
    
    // Upsample with bilinear + sharpen
    vec2 offset = texelSize * 0.5;
    vec3 color = low * 0.5;
    color += texture(lowResTex, texCoord + vec2( 0.5,  0.5) * offset).rgb * 0.125;
    color += texture(lowResTex, texCoord + vec2(-0.5,  0.5) * offset).rgb * 0.125;
    color += texture(lowResTex, texCoord + vec2( 0.5, -0.5) * offset).rgb * 0.125;
    color += texture(lowResTex, texCoord + vec2(-0.5, -0.5) * offset).rgb * 0.125;
    
    vec3 high = texture(highResTex, texCoord).rgb;
    FragColor = vec4(mix(high, color * intensity, 1.0), 1.0);
}
