#version 400
uniform mat4 modelViewMatrix; // oF Default
uniform mat4 modelViewProjectionMatrix; // oF Default
in vec4 position; // oF Default
in vec2 texcoord; // oF Default

uniform sampler2DRect bokehPosDepthCoc;
uniform sampler2DRect bokehColor;
uniform float bokehDepthCutoff;
uniform float maxBokehRadius;

out vec4 vColor;
out vec2 vTexCoord;

void main() {

    // Instanced rendering
    ivec2 sampleUv = ivec2(gl_InstanceID, 0);
    vec4 s = texelFetch(bokehPosDepthCoc, sampleUv);

    vec2 centerPos = s.xy;
    float depth = s.b;
    float coc = s.a;
    vec4 color = texelFetch(bokehColor, sampleUv);

    float size = maxBokehRadius * max(coc * depth, 0.1);
    vec3 p = position.xyz * size + vec3(centerPos, 0.);

    float alpha = clamp(coc - bokehDepthCutoff, 0., 1.);

    vColor.rgb = color.rgb;
    vColor.a = alpha;
    vTexCoord = texcoord;
    gl_Position = modelViewProjectionMatrix * vec4(p, 1.);
}
