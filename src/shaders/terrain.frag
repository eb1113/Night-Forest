#version 330 core
in float vHeight;
out vec4 fragColor;

void main() {
    // darker base colors
    vec3 dirt  = vec3(0.25, 0.20, 0.10);  // dark brown
    vec3 grass = vec3(0.05, 0.40, 0.05);  // deep green
    vec3 rock  = vec3(0.3, 0.3, 0.3);     // dark gray

    // match your TileMap amplitude
    float maxHeight = 1.5;
    float dirtLimit  = maxHeight * 0.33;
    float grassLimit = maxHeight * 0.66;

    vec3 color;

    if(vHeight < dirtLimit)
        color = dirt;
    else if(vHeight < grassLimit)
        color = mix(dirt, grass, (vHeight - dirtLimit)/(grassLimit - dirtLimit));
    else
        color = mix(grass, rock, (vHeight - grassLimit)/(maxHeight - grassLimit));

    fragColor = vec4(color, 1.0);
}