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


// logic for fireflies could not work
uniform vec3 lightPos[MAX_LIGHTS];
uniform vec3 lightColorArr[MAX_LIGHTS];
uniform float lightRadius[MAX_LIGHTS];

uniform sampler2D treeTexture;

void main()
{
    vec4 texColor = texture(treeTexture, TexCoord);
    if (texColor.a < 0.1)
        discard;

    vec3 norm = normalize(Normal);
    vec3 albedo = texColor.rgb;
    float foliageMask = smoothstep(0.18, 0.55, albedo.g - max(albedo.r, albedo.b));

    // Keep a small amount of moonlight-like fill so unlit leaves do not go fully black.
    //changed they are fully black now, but keeping the comment to maybe come back to
    vec3 ambientLight = vec3(0.0,0.0,0.0);

    // Basic diffuse
    vec3 L = normalize(spotPos - FragPos);
    float diff = max(dot(norm, L), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(L + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 18.0);

    vec3 diffuseLight  = diff * lightColor * 0.16;
    vec3 specularLight = vec3(0.08) * spec * (1.0 - foliageMask * 0.65);

    // FLASHLIGHT 
    float theta = dot(L, normalize(-spotDir));
    float epsilon = innerCutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);
    float flashlightDistance = length(spotPos - FragPos);
    float flashlightFalloff = 1.0 / (1.0 + 0.00035 * flashlightDistance + 0.0012 * flashlightDistance * flashlightDistance);
    vec3 flashlightLighting = (diffuseLight + specularLight) * intensity * flashlightFalloff;

    //logic for the fireflies we will see if this works
    vec3 pointLightTotal = vec3(0.0);

    for(int i = 0; i < numLights; i++){
        vec3 PL = lightPos[i] - FragPos;
        float dist = length(PL);
        PL = normalize(PL);

        float pdiff = max(dot(norm, PL), 0.0);
        float radius = max(lightRadius[i], 0.001);
        float normalizedDistance = clamp(dist / radius, 0.0, 1.0);
        float attenuation = pow(1.0 - normalizedDistance, 2.0);
        vec3 pointColor = mix(lightColorArr[i], vec3(dot(lightColorArr[i], vec3(0.299, 0.587, 0.114))), 0.35);

        pointLightTotal += pdiff * pointColor * attenuation * 0.65;
    }

    vec3 lighting = ambientLight + flashlightLighting + pointLightTotal;
    vec3 litColor = albedo * lighting;

    // Compress bright spikes so leaves keep detail 
    litColor = litColor / (vec3(1.0) + litColor);
    litColor = pow(litColor, vec3(1.0 / 1.15));

    FragColor = vec4(litColor, texColor.a);
}
