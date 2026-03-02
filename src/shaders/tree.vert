#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

layout (location = 3) in vec3 instancePos;
layout (location = 4) in float instanceRot;
layout (location = 5) in float instanceScale;

uniform mat4 view;
uniform mat4 projection;

mat4 makeTranslation(vec3 t) {
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        t.x, t.y, t.z, 1.0
    );
}

mat4 makeScale(float s) {
    return mat4(
        s,   0.0, 0.0, 0.0,
        0.0, s,   0.0, 0.0,
        0.0, 0.0, s,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 makeRotationY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
         c, 0.0,  s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        -s, 0.0,  c, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main()
{
    mat4 model = makeTranslation(instancePos)
               * makeRotationY(instanceRot)
               * makeScale(instanceScale);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
