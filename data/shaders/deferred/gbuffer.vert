#version 400
uniform mat4 modelViewMatrix; // oF Default
uniform mat4 modelViewProjectionMatrix; // oF Default
uniform mat4 normalMatrix; // Pass from C++
in vec4 position; // oF Default
in vec4 normal; // oF Default
in vec4 color; // oF Default
in vec2 texcoord; // oF Default

uniform vec4 globalColor;
uniform float lds; // lenear depth scalar

out block {
    vec4 viewPos;
    float depth;
    vec3 normal;
    vec2 texcoord;
    vec4 color;
} Out;

void main(){
    gl_Position = modelViewProjectionMatrix * position;

    Out.viewPos = modelViewMatrix * position;
    Out.depth = - Out.viewPos.z * lds;
    Out.normal = (normalMatrix * normal).xyz;
    Out.texcoord = texcoord;
    Out.color = color * globalColor;
}
