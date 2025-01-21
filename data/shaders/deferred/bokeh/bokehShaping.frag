#version 400
uniform sampler2DRect tex;
uniform int shapeId;
in vec2 vTexCoord;
out vec4 outputColor;

const float PI = 3.14159265359;
const float TWO_PI = 2. * PI;

float polygon(in vec2 uv, int n) {
    // https://thebookofshaders.com/07/
    // Angle and radius from the current pixel
    float a = atan(uv.x, uv.y)+PI;
    float r = TWO_PI / float(n);

    // Shaping function that modulate the distance
    return cos(floor(.5 + a / r) * r - a) * length(uv);
}

float circle(in vec2 uv) {
    return length(uv);
}

void main(){
    vec2 res = textureSize(tex);
    vec2 uv = vTexCoord / res * 2. - 1.;
    float d = 0;

    if (shapeId == 0) d = circle(uv);
    else if (shapeId == 1) d = polygon(uv, 5);
    else if (shapeId == 2) d = polygon(uv, 6);

    outputColor.r = 1. - smoothstep(.5, .9, d);
    outputColor.a = 1.;
}
