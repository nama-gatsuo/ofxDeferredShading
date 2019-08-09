#version 400

uniform sampler2DRect bokehTex;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 outputColor;

void main(){
    float c = texture(bokehTex, vTexCoord * textureSize(bokehTex)).r;
    if (c == 0.) discard;
    outputColor = c * vColor;
}
