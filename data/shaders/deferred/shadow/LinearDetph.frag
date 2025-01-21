#version 400

in block {
    vec4 viewPos;
    float depth;
    vec3 normal;
    vec2 texcoord;
    vec4 color;
} In;

out vec4 outputColor;

void main(){
    outputColor = vec4(vec3(In.depth), 1.0);
}
