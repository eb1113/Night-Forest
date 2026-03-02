#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

void main()
{
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 baseColor = vec3(0.0, 0.8, 0.0); // green for tree
    FragColor = vec4(baseColor * diff, 1.0);
}
