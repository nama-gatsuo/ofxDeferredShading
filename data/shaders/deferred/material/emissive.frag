#version 400
in vec4 vPosition;
in float vDepth;
in vec2 vTexCoord;
in vec4 vColor;

uniform int isShadow;

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
    if (isShadow == 1) {
        outputColor0.r = vDepth;
        outputColor0.a = 1.0;
    } else {
        outputColor0 = vColor;
        outputColor1 = vPosition;
        outputColor2 = vec4(calcFlatNormal(vPosition.xyz), vDepth);
    }

}
