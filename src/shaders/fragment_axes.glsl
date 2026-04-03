#version 400 core

in vec3 fragColor;
in float fragAlpha;
out vec4 screenColor;

void main(){
    screenColor = vec4(fragColor, fragAlpha);
}