#version 400

uniform int horizontal;
uniform sampler2DRect tex;

in vec2 vTexCoord;
out vec4 outputColor;

const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main(){

    vec2 texOffset = vec2(3.0);
    vec3 result = texture(tex, vTexCoord).rgb * weight[0];

    if (horizontal == 1) {
        for (int i = 0; i < 5; i++) {
            result += texture(tex, vTexCoord + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(tex, vTexCoord - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
        }
    } else {
        for (int i = 0; i < 5; i++) {
            result += texture(tex, vTexCoord + vec2(0.0, texOffset.y * i)).rgb * weight[i];
            result += texture(tex, vTexCoord - vec2(0.0, texOffset.y * i)).rgb * weight[i];
        }
    }

    outputColor = vec4(result, 1.0);

}
