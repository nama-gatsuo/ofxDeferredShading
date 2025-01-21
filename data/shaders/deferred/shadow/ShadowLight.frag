#version 400
precision highp float;

uniform sampler2DRect tex;
uniform sampler2DRect lightDepthTex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;

uniform mat4 shadowTransMat;
uniform vec3 lightDir;
uniform vec2 sun;

uniform vec4 ambient;
uniform vec4 diffuse;
// uniform vec4 specular;
uniform float darkness;
uniform float lds;
uniform float near;
uniform float biasScalar;

uniform float fovy;
uniform float farClip;

uniform int isDrawSun;
uniform int isShading;
uniform int isVolume;
uniform float scattering;
uniform float volumeStrength;

in vec2 vTexCoord;
out vec4 outputColor;

float sampleShadow(in vec2 uv, float compare) {
    return 1. - smoothstep(compare - 0.01, compare + 0.01, texture(lightDepthTex, uv).r - near * lds);
}

float sampleShadowLinear(in vec2 uv, float compare) {
    vec2 pixPos = uv + vec2(0.5);
    vec2 fracPart = fract(pixPos);
    vec2 startPos = pixPos - fracPart;

    float bl = sampleShadow(startPos, compare);
    float br = sampleShadow(startPos + vec2(1., 0.), compare);
    float tl = sampleShadow(startPos + vec2(0., 1.), compare);
    float tr = sampleShadow(startPos + vec2(1., 1.), compare);

    float mixL = mix(bl, tl, fracPart.y);
    float mixR = mix(br, tr, fracPart.y);
    return mix(mixL, mixR, fracPart.x);
}

float sampleShadowPCF(in vec2 uv, float compare) {
    const float sampleNum = 5.;
    const float sampleStart = (sampleNum - 1.) / 2.;
    const float sampleSqrd = sampleNum * sampleNum;

    float result = 0.;
    for (float x = - sampleStart; x <= sampleStart; x += 1.) {
        for (float y = - sampleStart; y <= sampleStart; y += 1.) {
            result += sampleShadowLinear(uv + vec2(x, y), compare);
        }
    }
    return result / sampleSqrd;
}

const float PI = 3.14159265359;
const mat4 bayerMat = mat4(
    0.0f, 0.5f, 0.125f, 0.625f,
    0.75f, 0.22f, 0.875f, 0.375f,
    0.1875f, 0.6875f, 0.0625f, 0.5625,
    0.9375f, 0.4375f, 0.8125f, 0.3125
);

float computeScattering(float lightDotView) {
    float result = 1.0f - scattering * scattering;
    result /= (PI * pow(1.0f + scattering * scattering - (2.0f * scattering) * lightDotView, 1.5f));
    return result;
}

vec3 accumurateFog(in vec3 rayGoal) {
    vec2 res = textureSize(lightDepthTex);
    vec3 accumFog = vec3(0.);
    float ditherValue = bayerMat[int(mod(vTexCoord.x, 4.f))][int(mod(vTexCoord.y, 4.f))];

    int sampleNum = 10;
    vec3 step = rayGoal / float(sampleNum);
    for (int i = 0; i < sampleNum; i++) {
        vec3 p = step * (float(i) + ditherValue);
        vec4 shadowCoord = shadowTransMat * vec4(p, 1);
        float dist = shadowCoord.z;
        if (texture(lightDepthTex, shadowCoord.xy * res).r - near * lds > dist) {
            accumFog += computeScattering(dot(vec3(0, 0, 1), lightDir)) * ambient.rgb;
        }
    }
    return accumFog * 1.2f / float(sampleNum);
}

void main() {

    vec4 read = texture(tex, vTexCoord);
    vec4 position = texture(positionTex, vTexCoord); // in view space

    // Sky area
    if (position.a == 0.0) {
        float sunLight = 0.;
        if (isDrawSun == 1) {
            float r = distance(vTexCoord, sun);
            const float sunRadius = 100.f;
            if (r < sunRadius) {
                sunLight = 3.f * pow(1.f - r / sunRadius, 2.f);
            }
        }

        vec3 fog = vec3(0.);
        if (isVolume == 1) {
            vec2 aspect = textureSize(tex);
            vec2 uv = vTexCoord / aspect - 0.5;
            uv.y = - uv.y;

            // Caluculate ray to the farClip
            vec3 ray;
            ray.x = uv.x * aspect.x / aspect.y;
            ray.y = uv.y;
            ray.z = - .5 / tan(fovy * 0.5);
            ray = ray / abs(ray.z) * farClip;
            fog = accumurateFog(ray);
        }

        outputColor = read + sunLight;
        outputColor.rgb += fog * volumeStrength;
        outputColor.a = 1.;
        return;
    }

    vec3 normal = texture(normalAndDepthTex, vTexCoord).rgb; // in view space

    // Hear is shadow map algorithm.
    // vertex in light space = bias * depthMVP * Inv(ModelView) * vertInViewSpace
    vec4 shadowCoord = shadowTransMat * position;
    float dist = shadowCoord.z;
    vec2 res = textureSize(lightDepthTex);

    float cosThete = clamp(dot(lightDir, normal), 0., 1.);
    float bias = biasScalar * clamp(tan(acos(cosThete)), 0., 1.);
    float sm = sampleShadowPCF(shadowCoord.xy * res, dist - bias);
    float shadow = (1. - sm * darkness);

    outputColor.rgb = read.rgb * shadow;
    if (isShading == 1) outputColor.rgb *= (ambient.rgb + diffuse.rgb * clamp(dot(normal, -lightDir), 0., 1.));
    if (isVolume == 1) outputColor.rgb += accumurateFog(position.xyz) * volumeStrength;
    outputColor.a = 1.0;
}
