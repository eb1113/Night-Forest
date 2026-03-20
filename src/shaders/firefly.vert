#version 330 core

layout (location = 0) in vec2 quadPos;  
layout (location = 1) in vec3 instancePos; // firefly world position

uniform mat4 view;
uniform mat4 projection;
uniform float size;

void main()
{
    // remove camera rotation so quad always faces camera
    mat4 billboard = mat4(mat3(view));

    vec3 worldPos = instancePos + (billboard * vec4(quadPos * size, 0.0, 0.0)).xyz;

    gl_Position = projection * view * vec4(worldPos, 1.0);
}
