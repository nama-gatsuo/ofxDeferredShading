#version 400

uniform sampler2DRect tex;
uniform sampler2DRect colorTex;
uniform sampler2DRect positionTex;
uniform sampler2DRect normalAndDepthTex;

#define maxlLightNum 32

uniform int lightNum;

uniform float lAttenuation;
uniform float lIntensity[maxlLightNum];
uniform float lRadius[maxlLightNum];
uniform vec3 lPosition[maxlLightNum];
uniform vec4 lDiffuse[maxlLightNum];
uniform vec4 lSpecular[maxlLightNum];

in vec2 vTexCoord;
out vec4 outputColor;

void main(){

    // float stencil = texture(colorTex, vTexCoord).a;
    // if (stencil < 0.001) {
    //     outputColor = vec4(0.);
    //     return;
    // }

    vec3 position = texture(positionTex, vTexCoord).rgb;
    vec3 N = texture(normalAndDepthTex, vTexCoord).rgb;
    vec3 V = normalize(position);

    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    for (int i = 0; i < maxlLightNum; i++) {
        if (i == lightNum) break;

        vec3 L = normalize(lPosition[i] - position);
        vec3 R = normalize(reflect(L, N));
        float lambert = max(dot(L, N), 0.0);
        float dist = distance(lPosition[i], position);

        if (dist < lRadius[i]) {
            float distPercent = dist / lRadius[i];
            float dampingFactor = 1.0 - pow(distPercent, 2.0);
            float attenuation = (1.0 / lAttenuation) * dampingFactor;
            float specularPower = pow(max(dot(R, V), 0.0), 32.0);

            diffuse += lDiffuse[i] * lambert * lIntensity[i] * attenuation;
            specular += lSpecular[i] * specularPower * lIntensity[i] * attenuation;
        }
    }

    outputColor = texture(tex, vTexCoord);
    //outputColor = vec4(0);
    outputColor += diffuse + specular;
    //outputColor = clamp(outputColor, vec4(0.), vec4(1.));
    //

    //tputColor.a = 1.;
}
