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
uniform float linearDepthScalar;

in vec2 vTexCoord;
out vec4 outputColor;

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
    float dist = shadowCoord.z; //

    float shadow = 1.0;

    vec2 res = textureSize(lightDepthTex);
    float texel = texture(lightDepthTex, shadowCoord.xy*res).r;

    float bias = 0.005;
    if (texel < dist - bias) shadow -= darkness;

    outputColor = read * (ambient + diffuse * dot(normal, -lightDir)) * shadow;
    outputColor.a = 1.0;
}
