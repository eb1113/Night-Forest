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
    vec3 norm = normalize(Normal);

    // Basic diffuse
    vec3 L = normalize(spotPos - FragPos);
    float diff = max(dot(norm, L), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(L + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    vec3 ambientLight  = 0.15 * lightColor;
    vec3 diffuseLight  = diff * lightColor * .1;
    vec3 specularLight = 0.3 * spec * lightColor;

    // FLASHLIGHT 
    float theta = dot(L, normalize(-spotDir));
    float epsilon = innerCutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

    vec3 flashlightLighting = (ambientLight + diffuseLight + specularLight) * intensity;

    //logic for the fireflies we will see if this works
    vec3 pointLightTotal = vec3(0.0);

    for(int i = 0; i < numLights; i++){
        vec3 PL = lightPos[i] - FragPos;
        float dist = length(PL);
        PL = normalize(PL);

        float pdiff = max(dot(norm, PL), 0.0);

        float attenuation = 1.0 / (1.0 + (dist * dist) / (lightRadius[i] * lightRadius[i]));

        pointLightTotal += pdiff * lightColorArr[i] * attenuation;
    }

    vec3 lighting = flashlightLighting + pointLightTotal;

    vec4 texColor = texture(treeTexture, TexCoord);
    if (texColor.a < 0.1)
        discard;

    FragColor = vec4(texColor.rgb * lighting, texColor.a);
}
