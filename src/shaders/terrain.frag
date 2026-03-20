#version 330 core
in float vHeight;
out vec4 fragColor;

void main() {
    vec3 dirt  = vec3(0.12, 0.09, 0.05);  // dark soil
    vec3 moss  = vec3(0.05, 0.18, 0.07);  // deep moss green
    vec3 grass = vec3(0.08, 0.22, 0.08);  // forest grass

    float maxHeight = 1.5;
    float dirtLimit = maxHeight * 0.33;
    float mossLimit = maxHeight * 0.66;

    vec3 color;

    if (vHeight < dirtLimit) {
        color = dirt;
    }
    else if (vHeight < mossLimit) {
        float t = (vHeight - dirtLimit) / (mossLimit - dirtLimit);
        color = mix(dirt, moss, t);
    }
    else {
        float t = (vHeight - mossLimit) / (maxHeight - mossLimit);
        color = mix(moss, grass, t);
    }

    // global darkening factor
    color *= 0.4;

    fragColor = vec4(color, 1.0);
}
