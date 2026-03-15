#version 330
in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D lowResTex;
uniform sampler2D highResTex;
uniform vec2 texelSize;

void main() {
    vec3 low = texture(lowResTex, texCoord).rgb;
    vec3 high = texture(highResTex, texCoord).rgb;
    
    // Upsample with gaussian
    vec3 color = low * 0.5;
    color += texture(lowResTex, texCoord + vec2(-texelSize.x, 0)).rgb * 0.125;
    color += texture(lowResTex, texCoord + vec2(texelSize.x, 0)).rgb * 0.125;
    color += texture(lowResTex, texCoord + vec2(0, -texelSize.y)).rgb * 0.125;
    color += texture(lowResTex, texCoord + vec2(0, texelSize.y)).rgb * 0.125;
    
    FragColor = vec4(high + color, 1.0);
}

