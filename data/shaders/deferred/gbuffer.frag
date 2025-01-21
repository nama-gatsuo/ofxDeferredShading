#version 400

in block {
    vec4 viewPos;
    float depth;
    vec3 normal;
    vec2 texcoord;
    vec4 color;
} In;

uniform int isShadow;

layout (location = 0) out vec4 outputColor0;
layout (location = 1) out vec4 outputColor1;
layout (location = 2) out vec4 outputColor2;
layout (location = 3) out vec4 outputColor3;

// vec3 calcFlatNormal(vec3 posInViewSpace){
//     vec3 dx = dFdx(posInViewSpace);
//     vec3 dy = dFdy(posInViewSpace);
//     vec3 n = normalize(cross(normalize(dx), normalize(dy)));
//     return - n;
// }

void main(){
    if (isShadow == 1) {
        outputColor0 = vec4(In.depth, 0., 0., 1.);
    } else {
        outputColor0 = In.color;
        outputColor1 = In.viewPos;
        outputColor2 = vec4(In.normal, In.depth);
    }
}
