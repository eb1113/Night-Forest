#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuseTex;

uniform vec3 lightDir;     // direction TO the light
uniform vec3 lightColor;   // usually white
uniform vec3 viewPos;      // camera position

void main() {
    vec3 norm = normalize(Normal);

    // Ambient
    vec3 ambient = 0.2 * lightColor;

    // Diffuse (Lambert)
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular (Blinn–Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(-lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = 0.3 * spec * lightColor;

    // Texture
    vec3 color = texture(diffuseTex, TexCoord).rgb;

    FragColor = vec4((ambient + diffuse + specular) * color, 1.0);
}
