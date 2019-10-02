#version 400

uniform sampler2DRect tex;
uniform sampler2DRect pass0;
uniform sampler2DRect pass1;
uniform sampler2DRect pass2;
uniform sampler2DRect pass3;
uniform sampler2DRect pass4;
uniform sampler2DRect pass5;
//uniform sampler2DRect pass6;
//uniform sampler2DRect pass7;
uniform float strength;
uniform float weights[6];
in vec2 vTexCoord;
out vec4 outputColor;

void main(){
    vec3 col = texture(tex, vTexCoord).rgb;
    col += texture(pass0, vTexCoord / 2.).rgb * weights[0] * strength;
    col += texture(pass1, vTexCoord / 4.).rgb * weights[1] * strength;
    col += texture(pass2, vTexCoord / 8.).rgb * weights[2] * strength;
    col += texture(pass3, vTexCoord / 16.).rgb * weights[3] * strength;
    col += texture(pass4, vTexCoord / 32.).rgb * weights[4] * strength;
    col += texture(pass5, vTexCoord / 64.).rgb * weights[5] * strength;
    //col += texture(pass6, vTexCoord / 128.).rgb * weights[6] * strength;
    //col += texture(pass7, vTexCoord / 256.).rgb * weights[7] * strength;
    outputColor = vec4(col, 1.);
}
