#version 400

uniform sampler2DRect tex;

in vec2 vTexCoord;
out vec4 outputColor;

void main(){
    float a = texture(tex, vTexCoord).a;
    if (a == 0.) outputColor = vec4(1.);
    else outputColor = vec4(vec3(a), 1.0);
}
