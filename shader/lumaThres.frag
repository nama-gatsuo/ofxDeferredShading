#version 400

uniform sampler2DRect tex;
uniform sampler2DRect read;
uniform float lumaThres;

in vec2 vTexCoord;
out vec4 outputColor;

float luma(in vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    //return (color.r + color.g + color.b) * 0.3333333;
}

void main(){

    vec3 albedColor = texture(tex, vTexCoord).rgb;
    vec3 shadedColor = texture(read, vTexCoord).rgb;
    vec3 col = mix(shadedColor, albedColor, step(vec3(1.0001), albedColor));
    outputColor = vec4(col * max(luma(col) - lumaThres, 0.), 1.);
}
