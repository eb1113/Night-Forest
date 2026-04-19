#version 330 core

in vec2 ScreenUV;

out vec4 FragColor;

uniform mat4 invProjection;
uniform mat4 invViewRotation;
uniform float time;
uniform vec3 moonDirection;

float hash(vec3 p)
{
    p = fract(p * 0.3183099 + vec3(0.1, 0.2, 0.3));
    p *= 17.0;
    return fract(p.x * p.y * p.z * (p.x + p.y + p.z));
}

void main()
{
    vec2 clipXY = ScreenUV * 2.0 - 1.0;
    vec4 viewRay = invProjection * vec4(clipXY, 1.0, 1.0);
    vec3 rayDir = normalize((invViewRotation * vec4(normalize(viewRay.xyz / viewRay.w), 0.0)).xyz);

    float horizonT = clamp(rayDir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 horizonColor = vec3(0.03, 0.05, 0.09);
    vec3 zenithColor = vec3(0.003, 0.006, 0.02);
    vec3 skyColor = mix(horizonColor, zenithColor, smoothstep(0.0, 0.95, horizonT));

    float starField = hash(floor(rayDir * 420.0));
    float starMask = smoothstep(0.992, 0.9996, starField);
    float twinkle = 0.75 + 0.25 * sin(time * 1.7 + dot(rayDir, vec3(37.2, 61.7, 19.4)) * 40.0);
    vec3 starColor = mix(vec3(0.7, 0.78, 0.95), vec3(1.0), hash(floor(rayDir * 190.0) + 17.0));
    skyColor += starColor * starMask * twinkle * smoothstep(-0.15, 0.35, rayDir.y);

    vec3 moonDir = normalize(moonDirection);
    float moonDot = dot(rayDir, moonDir);
    float moonDisc = smoothstep(0.9972, 0.9988, moonDot);
    float moonGlow = smoothstep(0.988, 0.9983, moonDot);
    vec3 moonColor = vec3(0.92, 0.94, 1.0);
    skyColor += moonColor * moonGlow * 0.14;
    skyColor += moonColor * moonDisc * 0.8;

    float moonShade = smoothstep(0.9962, 0.999, dot(rayDir, normalize(moonDir + vec3(0.08, -0.04, 0.02))));
    skyColor -= vec3(0.08, 0.09, 0.12) * moonShade * moonDisc * 0.4;

    FragColor = vec4(skyColor, 1.0);
}
