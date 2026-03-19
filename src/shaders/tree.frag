#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D treeTexture;

void main()
{
    vec3 norm = normalize(Normal);

    float diff = max(dot(norm, -lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(-lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    vec3 ambientLight  = 0.15 * lightColor;
    vec3 diffuseLight  = diff * lightColor;
    vec3 specularLight = 0.3 * spec * lightColor;

    vec3 lighting = ambientLight + diffuseLight + specularLight;

    vec4 texColor = texture(treeTexture, TexCoord);

    if (texColor.a < 0.1)
        discard;

    vec3 finalColor = texColor.rgb * lighting;

    FragColor = vec4(finalColor, texColor.a);
}
