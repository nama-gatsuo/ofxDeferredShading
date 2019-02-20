#version 400
precision highp float;

uniform sampler2DRect tex;
uniform sampler2DRect colorTex;
uniform sampler2DRect lightDepthTex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;

uniform mat4 shadowTransMat;
uniform vec3 lightDir;

uniform vec4 ambient;
uniform vec4 diffuse;
// uniform vec4 specular;
uniform float darkness;
uniform float lds;
uniform float near;

in vec2 vTexCoord;
out vec4 outputColor;

float sampleShadow(in vec2 uv, float compare) {
    return step(texture(lightDepthTex, uv).r - near * lds, compare);
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

void main() {

    vec4 read = texture(tex, vTexCoord);
    float stencil = texture(colorTex, vTexCoord).a;
    if (stencil < 0.001) {
        outputColor = read;
        return;
    }
    vec4 position = texture(positionTex, vTexCoord); // in view space
    vec3 normal = texture(normalAndDepthTex, vTexCoord).rgb; // in view space

    // vertex in light space = bias * depthMVP * Inv(ModelView) * vertInViewSpace
    vec4 shadowCoord = shadowTransMat * position;
    float dist = shadowCoord.z;
    vec2 res = textureSize(lightDepthTex);

    float cosThete = clamp(dot(lightDir, normal), 0., 1.);
    float bias = 0.005 * tan(acos(cosThete));
    bias = clamp(bias, 0., 0.005);

    float sm = sampleShadowPCF(shadowCoord.xy * res, dist - bias);

    float shadow = (1. - sm * darkness);

    outputColor = read * (ambient + diffuse * clamp(dot(normal, -lightDir), 0., 1.)) * shadow;
    outputColor.a = 1.0;
}
