#version 400

uniform sampler2DRect tex;
uniform sampler2DRect lightDepthTex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;

uniform float linearDepthScalar;
uniform mat4 shadowTransMat;
uniform vec3 lightPosInViewSpace;

uniform vec4 ambient;
uniform vec4 diffuse;
// uniform vec4 specular;
uniform float darkness;
uniform float blend;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {

    vec4 read = texture(tex, vTexCoord);
    vec4 vertInViewSpace = texture(positionTex, vTexCoord); // in view space
    vec3 normal = texture(normalAndDepthTex, vTexCoord).rgb; // in view space

    vec3 lightDir = normalize(lightPosInViewSpace.xyz - vertInViewSpace.xyz);
    // vec3 R = normalize(reflect(lightDir, normal));
    // vec3 V = normalize(vertInViewSpace.xyz);

    float lambert = max(dot(normal, lightDir), 0.0);

    vec4 amb = vec4(vec3(0.0), 1.0), dif = vec4(vec3(0.0), 1.0);

    if (lambert > 0.0) {
        amb += ambient;
        dif += diffuse * lambert;
        // spe += specular * pow(max(dot(R,V), 0.0), 64);
    }

    vec4 vertInLightSpace = shadowTransMat * vertInViewSpace;
    float lightDepth = length(vertInViewSpace.xyz - lightPosInViewSpace) * linearDepthScalar;

    vec3 depth = vertInLightSpace.xyz / vertInLightSpace.w;
    float shadow = 1.0;

    vec2 res = textureSize(lightDepthTex);
    float sum = 0.;
    sum += texture(lightDepthTex, depth.xy * res + vec2(-1,-1)).r;
    sum += texture(lightDepthTex, depth.xy * res + vec2(-1,1)).r;
    sum += texture(lightDepthTex, depth.xy * res + vec2(1,1)).r;
    sum += texture(lightDepthTex, depth.xy * res + vec2(1,-1)).r;
    float texel = sum * 0.25;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    if (lightDepth - bias > texel) shadow = 1.0 - darkness;

    if (depth.x >= 1.0 || depth.y >= 1.0 || depth.z >= 1.0) shadow = 1.0;
    if (depth.x <= 0.0 || depth.y <= 0.0 || depth.z <= 0.0) shadow = 1.0;

    outputColor = mix(vec4(amb + dif), read, blend) * shadow;
    outputColor.a = 1.0;
}
