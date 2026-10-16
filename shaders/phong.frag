#version 330 core
in vec3 vWorldPos;
in vec3 vNormal;

uniform vec3  uViewPos;
uniform vec3  uLightPos;
uniform vec3  uLightColor;
uniform vec3  uColor;      // base material color
uniform float uAlpha;      // 1 = opaque, <1 = glass
uniform float uShininess;  // specular power (metal = high)
uniform float uSpecular;   // specular strength
uniform vec3  uEmissive;   // LED / glow
uniform float uHighlight;  // 0..1 pulse for the selected component

out vec4 FragColor;

void main() {
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vWorldPos);
    vec3 V = normalize(uViewPos - vWorldPos);
    vec3 H = normalize(L + V);                       // Blinn-Phong half vector

    float dist = length(uLightPos - vWorldPos);
    float atten = 1.0 / (1.0 + 0.045 * dist + 0.0075 * dist * dist);

    vec3 ambient  = 0.30 * uColor;
    vec3 fillDir  = normalize(vec3(-0.4, 0.8, 0.5));      // soft fill light so shadows are never pitch black
    vec3 fill     = max(dot(N, fillDir), 0.0) * uColor * 0.55;
    vec3 diffuse  = max(dot(N, L), 0.0) * uColor * uLightColor;
    float spec    = pow(max(dot(N, H), 0.0), uShininess) * uSpecular;
    vec3 specular = spec * uLightColor;

    vec3 result = ambient + fill + (diffuse + specular) * atten + uEmissive + vec3(0.15, 0.45, 1.0) * uHighlight;
    FragColor = vec4(result, uAlpha);
}
