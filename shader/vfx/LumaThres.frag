#version 400

uniform sampler2DRect tex;
uniform float lumaThres;

in vec2 vTexCoord;
out vec4 outputColor;

float luma(vec3 color) {
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void main(){

    vec4 col = texture(tex, vTexCoord);
    outputColor = col * clamp(luma(col.rgb) - lumaThres, 0., 1.) * (1. / (1. - lumaThres));

}
