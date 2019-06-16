#version 400

uniform sampler2DRect tex;
uniform sampler2DRect blurredThres;

uniform float strength;
in vec2 vTexCoord;
out vec4 outputColor;

void main(){
    vec3 col = texture(tex, vTexCoord).rgb;
    vec3 bloom = texture(blurredThres, vTexCoord).rgb;
    col += bloom * strength;
    outputColor = vec4(col, 1.);
}
