#version 330 core
layout (location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex; //texture coordinates


out float vHeight; //send height to fragment shader
out vec2 vTexCoord;
out vec3 vWorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;
    vHeight = aPos.y;
    vTexCoord = aPos.xz * 0.08;
    vWorldPos = worldPos.xyz;
}
