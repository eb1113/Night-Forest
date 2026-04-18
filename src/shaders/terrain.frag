#version 330 core
#define MAX_LIGHTS 12

in float vHeight;
in vec2 vTexCoord;
in vec3 vWorldPos;

out vec4 fragColor;

uniform sampler2D groundTexture;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 spotPos;
uniform vec3 spotDir;
uniform float innerCutoff;
uniform float outerCutoff;
uniform int numLights;
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColorArr[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];

void main() {
    vec3 dirt  = vec3(0.12, 0.09, 0.05);
    vec3 moss  = vec3(0.05, 0.18, 0.07);
    vec3 grass = vec3(0.08, 0.22, 0.08);

    float maxHeight = 1.5;
    float dirtLimit = maxHeight * 0.33;
    float mossLimit = maxHeight * 0.66;

    vec3 color;

    if (vHeight < dirtLimit) {
        color = dirt;
    }
    else if (vHeight < mossLimit) {
        float t = (vHeight - dirtLimit) / (mossLimit - dirtLimit);
        color = mix(dirt, moss, t);
    }
    else {
        float t = (vHeight - mossLimit) / (maxHeight - mossLimit);
        color = mix(moss, grass, t);
    }

    vec3 groundSample = texture(groundTexture, vTexCoord).rgb;
    vec3 albedo = groundSample * mix(vec3(0.55), color * 2.2, 0.35);

    // Rebuild the surface normal from the lit world-space slope so hills react correctly.
    vec3 dpdx = dFdx(vWorldPos);
    vec3 dpdy = dFdy(vWorldPos);
    vec3 norm = normalize(cross(dpdx, dpdy));
    if (norm.y < 0.0) {
        norm = -norm;
    }

    vec3 L = normalize(spotPos - vWorldPos);
    float diff = max(dot(norm, L), 0.0);

    vec3 viewDir = normalize(viewPos - vWorldPos);
    vec3 halfwayDir = normalize(L + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 10.0);

    float theta = dot(L, normalize(-spotDir));
    float epsilon = innerCutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
    float flashlightDistance = length(spotPos - vWorldPos);
    float flashlightFalloff = 1.0 / (1.0 + 0.01 * flashlightDistance + 0.018 * flashlightDistance * flashlightDistance);

    vec3 flashlightLighting =
        (diff * lightColor * 0.22 + vec3(0.05) * spec) *
        intensity *
        flashlightFalloff;

    vec3 pointLightTotal = vec3(0.0);
    for (int i = 0; i < numLights; i++) {
        vec3 PL = lightPos[i] - vWorldPos;
        float dist = length(PL);
        PL = normalize(PL);

        float pdiff = max(dot(norm, PL), 0.0);
        float radius = max(lightRadius[i], 0.001);
        float normalizedDistance = clamp(dist / radius, 0.0, 1.0);
        float attenuation = pow(1.0 - normalizedDistance, 2.0);
        vec3 pointColor = mix(lightColorArr[i], vec3(dot(lightColorArr[i], vec3(0.299, 0.587, 0.114))), 0.45);

        pointLightTotal += pdiff * pointColor * attenuation * 0.5;
    }

    vec3 lighting = flashlightLighting + pointLightTotal;
    vec3 litColor = albedo * lighting;
    litColor = litColor / (vec3(1.0) + litColor);
    litColor = pow(litColor, vec3(1.0 / 1.1));

    fragColor = vec4(litColor, 1.0);
}
