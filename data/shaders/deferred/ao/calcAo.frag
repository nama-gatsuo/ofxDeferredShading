#version 400
uniform sampler2DRect colorTex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;
uniform sampler2DRect noiseTex;
uniform mat4 projectionMatrix;
uniform float radius;
uniform float darkness;
uniform vec3 ssaoKernel[32];

const int LoopNum = 32;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {

    if (texture(colorTex, vTexCoord).a == 0.0) {
        outputColor = vec4(1.);
        return;
    }

    vec2 res = textureSize(colorTex);

    vec3 P = texture(positionTex, vTexCoord).rgb;
    vec3 N = texture(normalAndDepthTex, vTexCoord).rgb;
    vec3 randomVec = texture(noiseTex, mod(vTexCoord, textureSize(noiseTex))).rgb;

    vec3 T = normalize(randomVec - N * dot(randomVec, N));
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);

    float occlusion = 0.0;

    for (int i = 0; i < LoopNum; i++) {

        vec3 samp = TBN * ssaoKernel[i];
        samp = P + samp * radius;

        vec4 offset = vec4(samp, 1.0);
        offset = projectionMatrix * offset;
        offset.xyz /= offset.w;
        offset.xy = (offset.xy * 0.5 + 0.5) * res;
        offset.y = res.y - offset.y;

        float sampleDepth = texture(positionTex, offset.xy).z;

        float bias = 0.025;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(P.z - sampleDepth));
        occlusion += (sampleDepth >= samp.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = clamp(1.0 - (occlusion / LoopNum), 0.0, 1.0);

    outputColor = vec4(vec3(occlusion), 1.);
}
