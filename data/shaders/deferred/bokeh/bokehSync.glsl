#version 440

layout(binding = 0, offset = 0) uniform atomic_uint bokehCounter;
layout(r32ui, binding = 1) writeonly uniform uimage1D indirectBufferTex;

uniform int maxCount;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
    int count = int(atomicCounter(bokehCounter));
    if (count > maxCount) count = maxCount;
    imageStore(indirectBufferTex, 1, uvec4(uint(count), 0, 0, 0));
}
