#version 400
in float vDepth;
out vec4 outputColor;
void main(){
    outputColor = vec4(vec3(vDepth), 1.0);
}
