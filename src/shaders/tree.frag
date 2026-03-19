#version 330 core

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

uniform sampler2D treeTexture;

void main()
{
    vec3 norm = normalize(Normal);

    // Basic diffuse (optional)
    vec3 L = normalize(spotPos - FragPos);
    float diff = max(dot(norm, L), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(L + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    vec3 ambientLight  = 0.15 * lightColor;
    vec3 diffuseLight  = diff * lightColor;
    vec3 specularLight = 0.3 * spec * lightColor;

    // FLASHLIGHT 
    float theta = dot(L, normalize(-spotDir));
    float epsilon = innerCutoff - outerCutoff;
    float intensity = clamp((theta - outerCutoff) / epsilon, 0.0, 1.0);

    vec3 lighting = (ambientLight + diffuseLight + specularLight) * intensity;

    vec4 texColor = texture(treeTexture, TexCoord);
    if (texColor.a < 0.1)
        discard;

    FragColor = vec4(texColor.rgb * lighting, texColor.a);
}
