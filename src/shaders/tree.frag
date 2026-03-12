#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightDir;      // direction TOWARD the light
uniform vec3 lightColor;    // usually vec3(1.0)
uniform vec3 viewPos;       // camera position
uniform vec3 materialColor; // the tree's actual color

void main()
{
    vec3 norm = normalize(Normal);

    // lighting white
    float diff = max(dot(norm, -lightDir), 0.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(-lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);

    // White light components
    vec3 ambientLight  = 0.15 * lightColor;
    vec3 diffuseLight  = diff * lightColor;
    vec3 specularLight = 0.3 * spec * lightColor;

    // Final color
    vec3 lighting = ambientLight + diffuseLight + specularLight;

    // Material color is applied AFTER lighting
    vec3 finalColor = materialColor * lighting;

    FragColor = vec4(finalColor, 1.0);
}
