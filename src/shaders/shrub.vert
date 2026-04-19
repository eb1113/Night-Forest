#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 3) in vec3 instancePos;
layout (location = 4) in float instanceRot;
layout (location = 5) in float instanceScale;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

mat4 makeModel(vec3 pos, float rot, float scaleVal)
{
    mat4 model = mat4(1.0);

    model[3] = vec4(pos, 1.0);

    float c = cos(rot);
    float s = sin(rot);
    mat4 rotY = mat4(
        vec4( c, 0, s, 0 ),
        vec4( 0, 1, 0, 0 ),
        vec4(-s, 0, c, 0 ),
        vec4( 0, 0, 0, 1 )
    );

    mat4 scaleM = mat4(
        vec4(scaleVal, 0, 0, 0),
        vec4(0, scaleVal, 0, 0),
        vec4(0, 0, scaleVal, 0),
        vec4(0, 0, 0, 1)
    );

    return model * rotY * scaleM;
}

void main()
{
    mat4 model = makeModel(instancePos, instanceRot, instanceScale);

    vec4 worldPos = model * vec4(aPos, 1.0);
    gl_Position = projection * view * worldPos;

    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
}
