#version 400
in vec4 vPosition;
in float vDepth;
in vec2 vTexCoord;
in vec4 vColor;

layout (location = 0) out vec4 outputColor0;
layout (location = 1) out vec4 outputColor1;
layout (location = 2) out vec4 outputColor2;
layout (location = 3) out vec4 outputColor3;

vec3 calcFlatNormal(vec3 posInViewSpace){
    vec3 dx = dFdx(posInViewSpace);
    vec3 dy = dFdy(posInViewSpace);
    vec3 n = normalize(cross(normalize(dx), normalize(dy)));

    return - n;
}

void main(){

    outputColor0 = vColor;
    outputColor1 = vPosition;
    outputColor2 = vec4(calcFlatNormal(vPosition.xyz), vDepth);

}
