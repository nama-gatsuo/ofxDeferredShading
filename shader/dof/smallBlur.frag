#version 400

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2DRect tex;

void main() {

    vec4 col = vec4(0.);
    col += texture(tex, vTexCoord + vec2(0.5, 0.5));
    col += texture(tex, vTexCoord + vec2(-0.5, 0.5));
    col += texture(tex, vTexCoord + vec2(0.5, -0.5));
    col += texture(tex, vTexCoord + vec2(-0.5, -0.5));
    col /= 4.;

    //outputColor = vec4(vec3(col.a), 1.);
    outputColor = col;
}
