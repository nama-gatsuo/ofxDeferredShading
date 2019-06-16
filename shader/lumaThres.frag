#version 400

uniform sampler2DRect tex;
uniform sampler2DRect read;
uniform float lumaThres;

in vec2 vTexCoord;
out vec4 outputColor;

float luma(in vec3 color) {
    //return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    return (color.r + color.g + color.b) * 0.3333333;
}

void main(){

    vec3 col0 = texture(tex, vTexCoord).rgb;
    vec3 col1 = texture(read, vTexCoord).rgb;
    vec3 col = mix(col1, col0, step(vec3(1.), col0));
    //outputColor = col * clamp(luma(col.rgb) - lumaThres, 0., 1.) * (1. / (max(lumaThres, 1.) + 0.0001));
    outputColor = vec4(col * max(luma(col) - lumaThres, 0.), 1.);
    //outputColor = col - vec4(vec3(1.), 0);
}
