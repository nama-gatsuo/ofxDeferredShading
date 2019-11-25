#version 400
uniform mat4 modelViewMatrix; // oF Default
uniform mat4 modelViewProjectionMatrix; // oF Default
uniform mat4 normalMatrix; // Pass from C++
in vec4 position; // oF Default
in vec4 normal; // oF Default
in vec4 color; // oF Default
in vec2 texcoord; // oF Default

uniform float lds; // lenear depth scalar
uniform vec4 emissiveColor;
uniform float intensity;
uniform float lightBrightness;
uniform vec4 clipPlane;
uniform mat4 invCamMat;

out vec4 vPosition;
out float vDepth;
out vec2 vTexCoord;
out vec4 vColor;

void main(){
    gl_Position = modelViewProjectionMatrix * position;

    vec4 viewPos = modelViewMatrix * position;
    vDepth = - viewPos.z * lds;
    vTexCoord = texcoord;
    vColor = emissiveColor * lightBrightness;
    vPosition = viewPos;
    gl_ClipDistance[0] = dot(clipPlane, invCamMat * viewPos);
}
