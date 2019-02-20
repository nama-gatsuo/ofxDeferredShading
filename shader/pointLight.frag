#version 400

uniform sampler2DRect colorTex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;

uniform vec3 lPosition;
uniform vec4 lDiffuse;
uniform vec4 lSpecular;
uniform float lAttenuation;
uniform float lIntensity;
uniform float lRadius;

in vec2 vTexCoord;
out vec4 outputColor;

void main(){

    float stencil = texture(colorTex, vTexCoord).a;
    if (stencil < 0.001) {
        outputColor = vec4(0.);
        return;
    }

    vec3 position = texture(positionTex, vTexCoord).rgb;
    float linearDepth = texture(normalAndDepthTex, vTexCoord).a;
    vec3 N = texture(normalAndDepthTex, vTexCoord).rgb;

    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    vec3 L = normalize(lPosition - position);
    vec3 R = normalize(reflect(L, N));
    vec3 V = normalize(position);

    float lambert = max(dot(L, N), 0.0);

    if (lambert > 0.0) {
        float dist = distance(lPosition, position);
        if (dist <= lRadius || lRadius > 0) {

            float distPercent = dist / lRadius;
            float dampingFactor = 1.0 - pow(distPercent, 2.0);
            float attenuation = (1.0 / lAttenuation) * dampingFactor;

            vec4 diffuseContribution = lDiffuse * lambert;
            diffuseContribution *= lIntensity;
            diffuseContribution *= attenuation;

            vec4 specularContribution = lSpecular * pow(max(dot(R, V), 0.0), 32.0);
            specularContribution *= lIntensity;
            specularContribution *= attenuation;

            diffuse += diffuseContribution;
            specular += specularContribution;

        }
    }

    outputColor = diffuse + specular;
    outputColor.a = 1.;
}
