#version 400
uniform mat4 modelViewProjectionMatrix; // oF Default
in vec4 position; // oF Default
in vec2 texcoord; // oF Default

out vec2 vTexCoord;

void main(){
    vTexCoord = texcoord;
    gl_Position = modelViewProjectionMatrix * position;
}
