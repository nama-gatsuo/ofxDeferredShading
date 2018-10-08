#version 400

uniform int isHorizontal;
uniform sampler2DRect tex;
uniform float sampleStep;
uniform int preShrink;
uniform float coefficients[32];
uniform int blurRes;

in vec2 vTexCoord;
out vec4 outputColor;

void main(){

    vec2 texOffset = vec2(.0);
    if (isHorizontal == 1) texOffset.x = 1.;
    else texOffset.y = 1.;

    vec2 uv = vTexCoord;
    // center
    vec4 result = texture(tex, uv) * coefficients[0];

    for (int i = 0; i < 32; i++) {
        if (i == blurRes) break;

        result += texture(tex, uv + vec2(texOffset * sampleStep * float(i + 1))) * coefficients[i + 1];
        result += texture(tex, uv - vec2(texOffset * sampleStep * float(i + 1))) * coefficients[i + 1];
    }

    outputColor = result;

}
