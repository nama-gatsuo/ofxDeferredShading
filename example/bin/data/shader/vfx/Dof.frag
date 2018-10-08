#version 400
#pragma include "../libs/random.frag"

in vec2 vTexCoord;
out vec4 outputColor;

uniform sampler2DRect tex;
uniform sampler2DRect normalAndDepthTex;

uniform float zNearStart;
uniform float zNearEnd;
uniform float zFarStart;
uniform float zFarEnd;
uniform float maxBlur;

uniform vec2 poisson[32];
const int nSamples = 24;

float mapCoc(float d) {
    return 1. - smoothstep(zNearEnd, zNearStart, d) * (1. - smoothstep(zFarStart, zFarEnd, d)); //* smoothstep(zEnd, 1., d);
}

void main() {

    vec3 color = texture(tex, vTexCoord).xyz;
    float depth = texture(normalAndDepthTex, vTexCoord).a; // linear depth 0 ~ 1.
    float blur = mapCoc(depth); // CoC 0 ~ 1
    float cocSize = blur * maxBlur;

    float theta = 2. * PI * srand(vTexCoord * 0.01);
    mat2 rot = mat2(
        cos(theta), sin(theta),
        - sin(theta), cos(theta)
    );

    float totalWeight = 0.;
    vec3 result = vec3(0);

    //poisson disk sampling with cocSize
    if (blur > 0.0) {
        for (int i = 0; i < nSamples; i++) {

            float radius = length(poisson[i]) * cocSize;
            vec2 uv = vTexCoord + (rot * poisson[i]) * cocSize;

            // sample color
            float depthSample = texture(normalAndDepthTex, uv).a;
            float blurSample = mapCoc(depthSample);
            vec3 colorSample = texture(tex, uv).rgb;

            //
            float cocWeight = clamp(cocSize + 1. - radius, 0., 1.);
            float depthWeight = float(depthSample >= depth);
            float tapWeight = cocWeight * clamp(depthWeight + blurSample, 0., 1.);

            result += colorSample * tapWeight;
            totalWeight += tapWeight;
        }

        result /= totalWeight;

    } else {
        result = color.rgb;
    }

    //result = vec3(blur);

    outputColor = vec4(result, 1.);
}
