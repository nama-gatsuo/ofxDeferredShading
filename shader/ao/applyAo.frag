#version 400

uniform sampler2DRect tex;
uniform sampler2DRect ssao;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    vec4 read = texture(tex, vTexCoord);
    float occlusion = pow(texture(ssao, vTexCoord).r, 4.);
    outputColor = vec4(read.rgb * occlusion, read.a);
}
