#version 330
in vec2 texCoord;
in vec2 rippleUV;
in vec3 worldPos;
in mat3 TBN;
out vec4 FragColor;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform vec3 lightDir;
uniform vec3 lightColor;

void main() {
    // Dynamic wet PBR override
    vec3 albedo = texture(albedoMap, texCoord).rgb;
    vec3 wetNormal = texture(normalMap, rippleUV).rgb * 2.0 - 1.0;
    vec3 N = normalize(TBN * wetNormal);
    vec3 L = normalize(lightDir);
    vec3 V = normalize(cameraPos - worldPos);  // assume uniform cameraPos
    
    float wet = 0.9;
    float NdotL = max(dot(N, L), 0.0);
    vec3 wetColor = albedo * lightColor * NdotL * wet;
    FragColor = vec4(wetColor, 1.0);
}

