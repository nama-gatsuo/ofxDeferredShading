#version 400

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2DRect tex;
uniform sampler2DRect midBlur;
uniform sampler2DRect largeBlur;
uniform sampler2DRect depthTex;

uniform float foculRangeStart;
uniform float foculRangeEnd;
uniform float nearEndCoc;
uniform float farEndCoc;

vec3 getSmallBlur(vec2 uv) {

    vec3 sum = vec3(0.);
    const float weight = 4.0 / 16.;
    sum += weight * texture(tex, uv + vec2(0.5, -1.5) ).rgb;
    sum += weight * texture(tex, uv + vec2(-1.5, -0.5) ).rgb;
    sum += weight * texture(tex, uv + vec2(-0.5, 1.5) ).rgb;
    sum += weight * texture(tex, uv + vec2(1.5, 0.5) ).rgb;
    return sum;
}

vec4 interpolateDof(vec3 noBlur, vec3 small, vec3 mid, vec3 large, float coc) {
    vec3 c = vec3(0.);

    float d0 = 0.2;
    float d1 = 0.4;
    float d2 = 0.4;

    if (coc < d0) {
        c = mix(noBlur, mid, coc / d0);
        //c = mix(small, mid, coc / d0);
    } else if (coc < d0 + d1) {
        c = mix(mid, large, (coc - d0) / d1);
    } else {
        c = large;
    }

    return vec4(c, 1.);
}

void main() {

    vec4 noBlur = texture(tex, vTexCoord);
    // vec3 small = getSmallBlur(vTexCoord);
    vec4 mid = texture(midBlur, vTexCoord / 4.);
    vec3 large = texture(largeBlur, vTexCoord / 4.).rgb;

    float nearCoc = mid.a;
    float coc = 0.;
    vec4 col = vec4(0.);

    float depth = texture(depthTex, vTexCoord).r;
    if (depth != 0.) {
        float a = farEndCoc / (1. - foculRangeEnd);
        float farCoc = clamp(a * (depth - foculRangeEnd), 0., farEndCoc);
        coc = max(nearCoc, farCoc);
    } else {
        col = noBlur;
        coc = farEndCoc;
    }
    //col = interpolateDof(noBlur.rgb, small, mid.rgb, large, coc);
    col = interpolateDof(noBlur.rgb, noBlur.rgb, mid.rgb, large, coc);

    outputColor = col;
    //outputColor = vec4(vec3(coc), 1.);
    //outputColor = texture(tex, vTexCoord);
}
