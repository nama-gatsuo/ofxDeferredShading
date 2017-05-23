// modified vesion of http://pasteall.org/10779 
#version 400

uniform sampler2DRect tex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;

uniform int numLights;
uniform vec3 lPosition;
uniform vec4 lAmbient;
uniform vec4 lDiffuse;
uniform vec4 lSpecular;
uniform float lAttenuation;
uniform float lIntensity;
uniform float lRadius;

in vec2 vTexCoord;
out vec4 outputColor;

const vec4 ambientGlobal = vec4(vec3(0.15), 1.0);

void main(){

    vec4 read = texture(tex, vTexCoord);
    vec3 position = texture(positionTex, vTexCoord).rgb;
    float linearDepth = texture(normalAndDepthTex, vTexCoord).a;
    vec3 normal = texture(normalAndDepthTex, vTexCoord).rgb;

    vec4 ambient = lAmbient;
    vec4 diffuse = vec4(vec3(0.0), 1.0);
    vec4 specular = vec4(vec3(0.0), 1.0);

    vec3 lightDir = lPosition - position;
    vec3 R = normalize(reflect(lightDir, normal));
    vec3 V = normalize(position);

    float lambert = max(dot(normal, normalize(lightDir)), 0.0);

    if (lambert > 0.0) {
        float dist = length(lightDir);
        if (dist <= lRadius || lRadius == 0.0) {

            float distPercent = lRadius == 0.0 ? 0.0 : dist / lRadius;
            float dampingFactor = 1.0 - pow(distPercent, 2.0);
            float attenuation = 1.0 / lAttenuation;
            attenuation *= dampingFactor;

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
    // gamma correction
    vec3 finalCol = pow(vec4(ambient + diffuse + specular).rgb, vec3(1.0/2.2));
    outputColor = vec4(finalCol, 1.0);

}
