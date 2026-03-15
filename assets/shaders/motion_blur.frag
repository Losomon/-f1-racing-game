#version 330
precision mediump float;

uniform sampler2D colorTex;
uniform sampler2D velocityTex;
uniform int numSamples = 12;
uniform float strength = 0.8;
uniform bool enabled = true;

in vec2 texCoord;
out vec4 FragColor;

void main() {
    if (!enabled) {
        FragColor = texture(colorTex, texCoord);
        return;
    }
    
    vec2 vel = texture(velocityTex, texCoord).xy * strength;
    vec3 sum = vec3(0.0);
    for (int i = 0; i < numSamples; ++i) {
        float t = (float(i) / float(numSamples - 1)) - 0.5;
        sum += texture(colorTex, texCoord + vel * t).rgb;
    }
    FragColor = vec4(sum / float(numSamples), 1.0);
}
