#version 400

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2DRect colorTex;
uniform sampler2DRect depthTex;
uniform float nearEndCoc;
uniform float foculRangeStart;

void main() {

    // bilinear filtering
    vec3 color = vec3(0);
    color += texture(colorTex, vTexCoord * 4. + vec2(0.5, 0.5)).rgb;
    color += texture(colorTex, vTexCoord * 4. + vec2(2.5, 0.5)).rgb;
    color += texture(colorTex, vTexCoord * 4. + vec2(0.5, 2.5)).rgb;
    color += texture(colorTex, vTexCoord * 4. + vec2(2.5, 2.5)).rgb;
    color /= 4.;

    vec4 cocUnit = vec4(0.);
    float a = - nearEndCoc / (foculRangeStart + 0.0001);

    float depth = texture(depthTex, vTexCoord * 4.).r;

    for (int i = 0; i < 4; i++) {
        vec4 d = vec4(0.);
        vec4 start = vec4(nearEndCoc);
        d.x = texture(depthTex, vTexCoord * 4. + vec2(0., float(i))).r;
        d.y = texture(depthTex, vTexCoord * 4. + vec2(1., float(i))).r;
        d.z = texture(depthTex, vTexCoord * 4. + vec2(2., float(i))).r;
        d.w = texture(depthTex, vTexCoord * 4. + vec2(3., float(i))).r;
        //d = vec4(1.) - d;
        //d = mix(vec4(0.), d, step(vec4(0.00001), d));
        start = mix(vec4(0.), vec4(nearEndCoc), step(vec4(0.0001), d));

        vec4 tempCocUnit = clamp(a * d + start, 0., nearEndCoc);
        cocUnit = max(tempCocUnit, cocUnit);
    }


    float coc = max(max(cocUnit.x, cocUnit.y), max(cocUnit.z, cocUnit.w));

    outputColor = vec4(color, coc);
}
