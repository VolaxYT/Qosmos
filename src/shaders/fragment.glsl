#version 400 core

in vec3 fragColor;
in float fragAlpha;
out vec4 screenColor;

void main(){
    vec2 coord = gl_PointCoord - vec2(0.5);
    float d = length(coord);
    float falloff = exp(-d * d * 1.5);

    if(d > 0.5) discard;

    screenColor = vec4(falloff * fragColor, fragAlpha);
}