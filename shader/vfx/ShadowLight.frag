#version 400
precision highp float;

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
    vec4 position = texture(positionTex, vTexCoord); // in view space
    vec3 normal = texture(normalAndDepthTex, vTexCoord).rgb; // in view space

    vec3 lightDir = normalize(lightPosInViewSpace.xyz - position.xyz);

    // vertex in light space = bias * depthMVP * Inv(ModelView) * vertInViewSpace
    vec4 shadowCoord = shadowTransMat * position;
    float dist = shadowCoord.z; //

    float shadow = 1.0;

    vec2 res = textureSize(lightDepthTex);
    float texel = texture(lightDepthTex, shadowCoord.xy*res).r;

    float bias = 0.0001 * tan(acos(dot(normal, lightDir)));
    if (texel < dist - bias) shadow -= darkness;

    outputColor = read * ambient + read * diffuse * dot(normal, lightDir) * shadow;
    outputColor.a = 1.0;
}
