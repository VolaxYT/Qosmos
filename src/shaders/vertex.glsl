#version 400 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aAlpha;

out vec3 fragColor;
out float fragAlpha;

uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * vec4(aPos, 1.0);

    float dist = length((view * vec4(aPos, 1.0)).xyz);
    float size_by_density = mix(10.0, 3.0, aAlpha * 6.0);
    float size_by_dist = 100.0 / dist;
    gl_PointSize = clamp(size_by_density * size_by_dist, 2.0, 15.0);

    fragColor = aColor;
    fragAlpha = aAlpha;
}