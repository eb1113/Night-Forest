#version 330 core

out vec4 FragColor;

uniform vec3 color;

void main()
{
    // soft circular glow
    float dist = length(gl_PointCoord - vec2(0.5));
    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);

    FragColor = vec4(color, alpha);
}
