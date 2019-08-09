#version 400
uniform sampler2DRect tex;
in vec2 vTexCoord;
out vec4 outputColor;
void main(){
    vec2 res = textureSize(tex);
    vec2 uv = vTexCoord / res * 2. - 1.;
    float dist = length(uv);

    outputColor.r = 1. - smoothstep(0.9, 1., dist);
    outputColor.a = 1.;
}
