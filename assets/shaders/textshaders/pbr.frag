precision mediump float;

// [Copy the full pbr.frag content from existing]
precision mediump float;

const float PI = 3.14159265359;

// ── Helpers ─────────────────────────────────────────────────────────────
// [Full content from previous read_file, to make complete]
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float d = NdotH * NdotH * (a2 - 1.0) + 1.0;
    return a2 / (PI * d * d);
}

float GeometrySchlickGGX(float Ndot, float roughness) {
    float r = roughness + 1.0;
    float k = (r*r) / 8.0;
    return Ndot / (Ndot * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0 * max(1.0 - roughness, 0.001));
}

// ── Direct lighting ──────────────────────────────────────────────────────
vec3 DirectLighting(vec3 N, vec3 V, vec3 L, vec3 albedo, float metallic, float roughness, vec3 lightColor) {
    vec3 H = normalize(V + L);
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;

    vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);
    return (kD * albedo / PI + specular) * max(dot(N, L), 0.0) * lightColor;
}

// ── Indirect lighting (IBL) ──────────────────────────────────────────────
vec3 IndirectLighting(vec3 N, vec3 V, vec3 albedo, float metallic, float roughness,
                      samplerCube irradianceMap, samplerCube prefilterMap,
                      sampler2D brdfLUT, float ao) {
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 kS = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo * kD;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 R = reflect(-V, N);
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F0 * envBRDF.x + envBRDF.y);

    return ((diffuse + specular) * ao);
}

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicRoughnessMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform mat3 TBN;
uniform vec3 cameraPos;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform float time;
uniform float ao;
uniform bool useWet;

in vec2 texCoord;
in vec3 worldPos;
in mat3 TBNMat;

void main() {
    vec3 albedo = texture(albedoMap, texCoord).rgb;
    vec2 mrSample = texture(metallicRoughnessMap, texCoord).rg;
    float metallic = mrSample.r;
    float roughness = mrSample.g;

    vec3 N;
    if (useWet) {
        metallic = 0.85;
        roughness = 0.08;
        vec3 rippleNormal = texture(normalMap, texCoord * 3.0 + vec2(time * 1.2, 0.0)).rgb * 2.0 - 1.0;
        N = normalize(TBNMat * normalize(rippleNormal));
    } else {
        vec3 normalMapSample = texture(normalMap, texCoord).rgb * 2.0 - 1.0;
        N = normalize(TBNMat * normalMapSample);
    }

    vec3 V = normalize(cameraPos - worldPos);

    vec3 Lo = vec3(0.0);
    Lo += DirectLighting(N, V, normalize(lightDir), albedo, metallic, roughness, lightColor);
    vec3 color = Lo + IndirectLighting(N, V, albedo, metallic, roughness, irradianceMap, prefilterMap, brdfLUT, ao);

    gl_FragColor = vec4(color, 1.0);  // HDR
}

