#version 400

uniform int horizontal;
uniform sampler2DRect tex;

uniform int sampleSize;
uniform float coefficients[33];
uniform float offsets[32];

in vec2 vTexCoord;
out vec4 outputColor;

const int NUM = 33;

void main(){

    vec2 texOffset = vec2(.0);
    if (horizontal == 1) {
        texOffset.x = 1.;
    } else {
        texOffset.y = 1.;
    }

    vec3 result = texture(tex, vTexCoord).rgb * coefficients[0];
    for (int i = 1; i < NUM; i++) {
        result += texture(tex, vTexCoord + vec2(texOffset * offsets[i-1])).rgb * coefficients[i];
        result += texture(tex, vTexCoord - vec2(texOffset * offsets[i-1])).rgb * coefficients[i];
    }

    outputColor = vec4(result, 1.0);

}
