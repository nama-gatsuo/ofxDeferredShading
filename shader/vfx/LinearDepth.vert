#version 400
uniform mat4 modelViewProjectionMatrix; // oF Default
uniform mat4 modelViewMatrix; // oF Default
in vec4 position; // oF Default

out vec4 vPosition;

void main(){
    gl_Position = modelViewProjectionMatrix * position;
    vPosition = modelViewMatrix * position;
}
