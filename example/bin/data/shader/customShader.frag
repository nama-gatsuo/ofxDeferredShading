#version 400
in vec4 vPosition;
in float vDepth;
in vec3 vNormal;
in vec4 vColor;

uniform int isShadow;

layout (location = 0) out vec4 outputColor0;
layout (location = 1) out vec4 outputColor1;
layout (location = 2) out vec4 outputColor2;
layout (location = 3) out vec4 outputColor3;

void main(){

    if (isShadow == 1) {
        outputColor0.r = vDepth;
        outputColor0.a = 1.0;
    } else {
        outputColor0 = vColor;
        outputColor1 = vPosition;
        outputColor2 = vec4(normalize(vNormal), vDepth);

        vec4 c = vec4(vec3(0.0), 1.0);
        if (dot(vColor, vColor) > dot(vec4(1.0), vec4(1.0))) c = vColor;
        outputColor3 = c;
    }
}
