#version 330
precision mediump float;

uniform sampler2D srcTex;
uniform vec2 texelSize;
uniform float level = 1.0; // 1,2,4,8,16

in vec2 texCoord;
out vec4 FragColor;

void main() {
    vec2 offset = texelSize * level;
    vec3 color = vec3(0.0);
    
    // 2x2 box filter
    color += texture(srcTex, texCoord + vec2( 0.0,  0.0) * offset).rgb * 0.25;
    color += texture(srcTex, texCoord + vec2( 1.0,  0.0) * offset).rgb * 0.25;
    color += texture(srcTex, texCoord + vec2( 0.0,  1.0) * offset).rgb * 0.25;
    color += texture(srcTex, texCoord + vec2( 1.0,  1.0) * offset).rgb * 0.25;
    
    FragColor = vec4(color, 1.0);
}
