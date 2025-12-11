#version 330 core
in float vHeight;
out vec4 fragColor;

void main() {
    //green
    vec3 color;

    if(vHeight < 1.0)
        color = vec3(0.4,0.3,0.1); //dirt
    else if(vHeight < 2.0)
        color = vec3(0.1,0.6,0.1); //grass
    else
        color = vec3(0.5,0.5,0.5); //gray rock

    fragColor = vec4(color, 1.0);
}