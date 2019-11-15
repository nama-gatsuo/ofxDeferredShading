#version 400

uniform sampler2DRect tex;
uniform sampler2DRect ssao;
uniform sampler2DRect colorTex;
uniform float darkness;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    vec4 read = texture(tex, vTexCoord);
    if (texture(colorTex, vTexCoord).a == 0.) {
        outputColor = read;
        return;
    }
    float occlusion = pow(texture(ssao, vTexCoord).r, 4.);
    outputColor = vec4(mix(read.rgb, read.rgb * occlusion, darkness), read.a);
}
