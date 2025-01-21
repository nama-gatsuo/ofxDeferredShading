#version 440

layout(binding = 0, offset = 0) uniform atomic_uint bokehCounter;
layout(rgba32f, binding = 0) uniform writeonly image2D bokehColor;
layout(rgba32f, binding = 1) uniform writeonly image2D bokehPosDepthCoc;
uniform sampler2DRect tex;
uniform sampler2DRect depthTex;

uniform float cocThres;
uniform float lumThres;

uniform float farEndCoc;
uniform float foculRangeEnd;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {

    vec2 uv = gl_GlobalInvocationID.xy;
    vec3 color = texture(tex, uv).rgb;

    vec3 avgColor = vec3(0);
    avgColor += texture(tex, uv + vec2(-1.5f, -1.5f)).rgb;
    avgColor += texture(tex, uv + vec2( 0.5f, -1.5f)).rgb;
    avgColor += texture(tex, uv + vec2( 1.5f, -1.5f)).rgb;
    avgColor += texture(tex, uv + vec2(-1.5f,  0.5f)).rgb;
    avgColor += texture(tex, uv + vec2( 0.5f,  0.5f)).rgb;
    avgColor += texture(tex, uv + vec2( 1.5f,  0.5f)).rgb;
    avgColor += texture(tex, uv + vec2(-1.5f,  1.5f)).rgb;
    avgColor += texture(tex, uv + vec2( 0.5f,  1.5f)).rgb;
    avgColor += texture(tex, uv + vec2( 1.5f,  1.5f)).rgb;
    avgColor /= 9.;

    float difLum = dot(vec3(1.), color) - dot(vec3(1.), avgColor);
    difLum /= 3.;

    float depth = texture(depthTex, uv).r;
    float a = farEndCoc / (1. - foculRangeEnd);
    float farCoc = clamp(a * (depth - foculRangeEnd), 0., farEndCoc);

    if (difLum > lumThres && depth > cocThres) {
        int current = int(atomicCounterIncrement(bokehCounter));
        imageStore(bokehPosDepthCoc, ivec2(current, 0), vec4(uv, depth, farCoc));
        imageStore(bokehColor, ivec2(current, 0), vec4(color, 1.));
    }

}
