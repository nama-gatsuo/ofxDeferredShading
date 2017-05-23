#version 400
uniform mat4 modelViewMatrix; // oF Default
uniform mat4 modelViewProjectionMatrix; // oF Default
uniform mat4 normalMatrix; // Pass from C++
in vec4 position; // oF Default
in vec4 normal; // oF Default
in vec4 color; // oF Default
in vec2 texcoord; // oF Default

uniform float farClip;
uniform float nearClip;

out vec4 vPosition;
out float vDepth;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vColor;

void main(){
    gl_Position = modelViewProjectionMatrix * position;

    vec4 viewPos = modelViewMatrix * position;
    vDepth = - viewPos.z / (farClip - nearClip);
    vNormal = (normalMatrix * normal).xyz;
    vTexCoord = texcoord;
    vColor = color;
    vPosition = viewPos;
}
