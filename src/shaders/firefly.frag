#version 330 core

in vec2 TexCoord;
in vec3 ParticleWorldPos;

out vec4 FragColor;

uniform vec3 color;
uniform float time;

void main()
{
    vec2 centeredUv = TexCoord - vec2(0.5);
    float dist = length(centeredUv);

    float outerGlow = 1.0 - smoothstep(0.18, 0.72, dist);
    float innerCore = 1.0 - smoothstep(0.02, 0.22, dist);
    float pulse = 0.72 + 0.28 * sin(time * 6.0 + ParticleWorldPos.x * 0.21 + ParticleWorldPos.z * 0.17);
    float alpha = outerGlow * 0.32 * pulse + innerCore * 0.9;

    if (alpha < 0.04) {
        discard;
    }

    vec3 haloColor = color * (0.55 + 0.45 * pulse);
    vec3 coreColor = mix(color, vec3(1.0, 0.97, 0.82), 0.5);
    vec3 finalColor = haloColor * outerGlow * 0.65 + coreColor * innerCore;

    FragColor = vec4(finalColor, min(alpha, 1.0));
}
