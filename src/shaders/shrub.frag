#version 330 core
#define MAX_LIGHTS 12

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

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

uniform sampler2D shrubTexture;
uniform sampler2D shrubAlphaTexture;

void main()
{
    vec4 texColor = texture(shrubTexture, TexCoord);
    float alpha = texture(shrubAlphaTexture, TexCoord).a;
    alpha = smoothstep(0.18, 0.82, alpha);

    if (alpha < 0.08)
        discard;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(faceforward(Normal, -viewDir, Normal));
    vec3 shrubTint = vec3(0.29, 0.52, 0.23);
    vec3 albedo = texColor.rgb * shrubTint;

    float foliageMask = smoothstep(0.08, 0.45, albedo.g - max(albedo.r, albedo.b));
    vec3 ambientLight = vec3(0.025, 0.035, 0.025);

    vec3 L = normalize(spotPos - FragPos);
    float diff = max(dot(norm, L), 0.0);
    vec3 halfwayDir = normalize(L + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 10.0);

    float theta = dot(L, normalize(-spotDir));
    float epsilon = innerCutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
    float flashlightDistance = length(spotPos - FragPos);
    float flashlightFalloff = 1.0 / (1.0 + 0.0005 * flashlightDistance + 0.0016 * flashlightDistance * flashlightDistance);

    float frontLight = max(dot(norm, L), 0.0);
    float backLight = max(dot(-norm, L), 0.0);
    vec3 translucency = lightColor * backLight * foliageMask * 0.18;
    vec3 diffuseLight = frontLight * lightColor * 0.18;
    vec3 specularLight = vec3(0.03) * spec * (1.0 - foliageMask * 0.85);
    vec3 flashlightLighting = (diffuseLight + specularLight + translucency) * intensity * flashlightFalloff;

    vec3 pointLightTotal = vec3(0.0);
    for (int i = 0; i < numLights; i++) {
        vec3 PL = lightPos[i] - FragPos;
        float dist = length(PL);
        PL = normalize(PL);

        float pdiff = max(dot(norm, PL), 0.0);
        float pback = max(dot(-norm, PL), 0.0);
        float radius = max(lightRadius[i], 0.001);
        float normalizedDistance = clamp(dist / radius, 0.0, 1.0);
        float attenuation = pow(1.0 - normalizedDistance, 2.0);
        vec3 pointColor = mix(lightColorArr[i], vec3(dot(lightColorArr[i], vec3(0.299, 0.587, 0.114))), 0.25);

        pointLightTotal += (pdiff * 0.55 + pback * foliageMask * 0.35) * pointColor * attenuation;
    }

    vec3 colorLift = mix(vec3(0.9, 0.95, 0.9), vec3(0.45, 0.72, 0.45), foliageMask * 0.62);
    vec3 lighting = ambientLight + flashlightLighting + pointLightTotal * 0.55;
    vec3 litColor = albedo * colorLift * lighting;
    litColor = litColor / (vec3(1.0) + litColor);
    litColor = pow(litColor, vec3(1.0 / 1.08));

    FragColor = vec4(litColor, alpha);
}
