#version 400

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2DRect shrunk;
uniform sampler2DRect shrunkBlurred;

void main() {

    vec4 sh = texture(shrunk, vTexCoord);
    vec4 shb = texture(shrunkBlurred, vTexCoord);
    float coc = 2. * max(sh.a, shb.a) - sh.a;

    outputColor = vec4(sh.rgb, coc);
}
