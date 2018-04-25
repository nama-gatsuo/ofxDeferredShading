#version 400

uniform sampler2DRect tex;

in vec2 vTexCoord;
out vec4 outputColor;

void main(){
    outputColor = vec4(vec3(texture(tex, vTexCoord).a), 1.0);
}
