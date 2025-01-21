#version 400

uniform sampler2DRect tex;
uniform int preShrink;

in vec2 vTexCoord;
out vec4 outputColor;

void main(){

    vec4 col = vec4(0);
    float num = float(preShrink * preShrink);

    for (int i = 0; i < 9; i++) {
        if (i >= preShrink) break;

        for (int j = 0; j < 9; j++) {
            if (j >= preShrink) continue;

            col += texture(tex, vTexCoord * float(preShrink) + vec2(float(i), float(j)));
        }

    }

    outputColor = col / num;

}
