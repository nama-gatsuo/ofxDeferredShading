#version 400

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2DRect colorTex;
uniform sampler2DRect normalAndDepthTex;
uniform float nearEndCoc;
uniform float foculRangeStart;

void main() {

    // bilinear filtering
    vec3 color = vec3(0);
    color += texture(colorTex, vTexCoord * 4. + vec2(0.5, 0.5) ).rgb;
    color += texture(colorTex, vTexCoord * 4. + vec2(2.5, 0.5) ).rgb;
    color += texture(colorTex, vTexCoord * 4. + vec2(0.5, 2.5) ).rgb;
    color += texture(colorTex, vTexCoord * 4. + vec2(2.5, 2.5) ).rgb;
    color /= 4.;

    vec4 cocUnit = vec4(0.);
    float a = - nearEndCoc / foculRangeStart;

    for (int i = 0; i < 4; i++) {
        vec4 d;
        d.x = texture(normalAndDepthTex, vTexCoord * 4. + vec2(0., float(i)) ).a;
        d.y = texture(normalAndDepthTex, vTexCoord * 4. + vec2(1., float(i)) ).a;
        d.z = texture(normalAndDepthTex, vTexCoord * 4. + vec2(2., float(i)) ).a;
        d.w = texture(normalAndDepthTex, vTexCoord * 4. + vec2(3., float(i)) ).a;

        vec4 tempCocUnit = clamp(a * d + nearEndCoc, 0., nearEndCoc);
        cocUnit = max(tempCocUnit, cocUnit);
    }

    float coc = max(max(cocUnit.x, cocUnit.y), max(cocUnit.z, cocUnit.w));

    outputColor = vec4(color, coc);
}
