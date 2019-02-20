#version 400

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2DRect tex;
uniform sampler2DRect midBlur;
uniform sampler2DRect largeBlur;
uniform sampler2DRect normalAndDepthTex;

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

vec4 interpolateDof(vec3 small, vec3 mid, vec3 large, float coc) {
    vec3 c = vec3(0.);

    float d0 = 0.4;
    float d1 = 0.4;
    float d2 = 0.2;

    if (coc < d0) {
        c = mix(small, mid, coc / d0);
    } else if (coc < d0 + d1) {
        c = mix(mid, large, (coc - d0) / d1);
    } else {
        c = large;
    }

    return vec4(c, 1.);
}

void main() {

    vec3 small = getSmallBlur(vTexCoord);
    vec4 mid = texture(midBlur, vTexCoord / 4.);
    vec3 large = texture(largeBlur, vTexCoord / 4.).rgb;

    float nearCoc = mid.a;
    float coc = 0.;
    vec4 col = vec4(0.);

    float depth = texture(normalAndDepthTex, vTexCoord).a;
    if (depth < 0.999) {
        float a = farEndCoc / (1. - foculRangeEnd);
        float farCoc = clamp(a * (depth - foculRangeEnd), 0., farEndCoc);

        coc = max(nearCoc, farCoc);

        if (coc == 0.0) {
            col = texture(tex, vTexCoord);
        } else {
            col = interpolateDof(small, mid.rgb, large, coc);
        }


    } else {
        col = texture(tex, vTexCoord);
        coc = 0.;
    }

    col.a = 1.;
    outputColor = col;
    //outputColor = vec4(vec3(large), 1.);
    //outputColor = texture(tex, vTexCoord);
}
