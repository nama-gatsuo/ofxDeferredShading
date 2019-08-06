#version 400

uniform sampler2DRect bokehTex;

in vec4 vColor;
in vec2 vTexCoord;

out vec4 outputColor;

void main(){
    outputColor = texture(bokehTex, vTexCoord * textureSize(bokehTex)).r * vColor;
    //outputColor = vec4(1.);
}
