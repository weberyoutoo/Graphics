#version 450 core

layout (local_size_x = 5, local_size_y = 1, local_size_z = 1) in ;
struct DrawCommand{
uint count ;
uint instanceCount ;
uint firstIndex ;
uint baseVertex ;
uint baseInstance ;
};
layout (std430, binding=3) buffer DrawCommandsBlock{
DrawCommand commands[] ;
};
void main() {
const uint idx = gl_LocalInvocationID.x;
commands[idx].instanceCount = 0 ;
}