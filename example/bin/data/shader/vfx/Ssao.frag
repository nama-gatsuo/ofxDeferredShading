#version 400
precision highp float;
#pragma include "../libs/random.frag"

uniform sampler2DRect tex;
uniform sampler2DRect colorTex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;
uniform mat4 projectionMatrix;
uniform float radius;
uniform float darkness;

const int LoopNum = 24;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    vec4 read = texture(tex, vTexCoord);
    float stencil = texture(colorTex, vTexCoord).a;
    if (stencil < 0.001) {
        outputColor = read;
        return;
    }
    vec4 position = texture(positionTex, vTexCoord);
    vec3 normal = texture(normalAndDepthTex, vTexCoord).rgb;
    float depth = texture(normalAndDepthTex, vTexCoord).a;

    vec2 size = textureSize(tex);

    float occlusion = 0.0;

    if (depth > 0.0) {
        for (int i = 0; i < LoopNum; i++) {

            vec3 s = position.xyz + (normal + getRandomVector(vec2(i*55.5, i*66.6))) * radius;

            vec4 offset = vec4(s, 1.0);
            offset = projectionMatrix * offset;
            offset.xyz /= offset.w;
            offset.xy = (offset.xy * 0.5 + 0.5) * size;
            offset.y = size.y - offset.y;

            float sampleDepth = texture(positionTex, offset.xy).z;

            float bias = 0.025;
            float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
            occlusion += (sampleDepth >= s.z + bias ? 1.0 : 0.0) * rangeCheck;
        }
        occlusion = clamp(1.0 - (occlusion / LoopNum), 0.0, 1.0);
    }
    outputColor = read * occlusion;
}
