#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex; //texture coordinates

out float vHeight; //send height to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vHeight = aPos.y;

}