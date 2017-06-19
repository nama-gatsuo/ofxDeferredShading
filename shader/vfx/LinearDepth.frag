#version 400

uniform float linearDepthScalar;

in vec4 vPosition;
out vec4 outputColor;

void main(){
    outputColor.r = - vPosition.z * linearDepthScalar;
    outputColor.a = 1.0;
}
