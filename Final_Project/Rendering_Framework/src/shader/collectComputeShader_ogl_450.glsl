#version 450 core

layout (local_size_x = 512, local_size_y = 1, local_size_z = 1) in ;
struct DrawCommand{
uint count ;
uint instanceCount ;
uint firstIndex ;
uint baseVertex ;
uint baseInstance ;
};
struct InstanceProperties{
mat4 position ;
};
struct InstancePropertiesPoint{
float point ;
};
layout (std430, binding=3) buffer DrawCommandsBlock{
DrawCommand commands[] ;
};
layout (std430, binding=1) buffer InstanceData{
InstanceProperties instanceProps[] ;
};
layout (std430, binding=2) buffer CurrValidInstanceDataPoint{
InstancePropertiesPoint currValidInstancePropsPoint[] ;
};

layout(location = 0) uniform mat4 mvp ;
//layout(location = 1) uniform mat4 viewMat ;
//layout(location = 2) uniform mat4 projMat ;
layout(location = 4) uniform vec3 rightFacenormal ;
layout(location = 5) uniform float rightFacedistance ;
layout(location = 6) uniform vec3 leftFacenormal ;
layout(location = 7) uniform float leftFacedistance ;
layout(location = 8) uniform vec3 topFacenormal ;
layout(location = 9) uniform float topFacedistance ;
layout(location = 10) uniform vec3 bottomFacenormal ;
layout(location = 11) uniform float bottomFacedistance ;
layout(location = 12) uniform vec3 nearFacenormal ;
layout(location = 13) uniform float nearFacedistance ;
layout(location = 14) uniform vec3 farFacenormal ;
layout(location = 15) uniform float farFacedistance ;
layout(location = 16) uniform int shift ;

void main() {
const uint bidx = gl_GlobalInvocationID.x + shift;;
uint idy = 0;
float sphereRadius = 0;
vec3 sphereCenter;
if(bidx<commands[1].baseInstance)
{
idy=0;
sphereRadius=1.4;
sphereCenter = vec3(0.0, 0.66, 0.0);
}
else if(bidx<commands[2].baseInstance)
{
idy=1;
sphereRadius=3.4;
sphereCenter = vec3(0.0, 2.55, 0.0);
}
else if(bidx<commands[3].baseInstance)
{
idy=2;
sphereRadius=2.6;
sphereCenter = vec3(0.0, 1.76, 0.0);
}
else if(bidx<commands[4].baseInstance)
{
idy=3;
sphereRadius=8.5;
sphereCenter = vec3(0.0, 4.57, 0.0);
}
else if(bidx<commands[5].baseInstance)
{
idy=4;
sphereRadius=10.2;
sphereCenter = vec3(0.0, 4.57, 0.0);
}
// put data into valid-instance buffer
// also update the instance count
if(bidx<commands[5].baseInstance )
{
    vec4 p = instanceProps[bidx].position*vec4(sphereCenter,1.0);
	vec4 pp = mvp*p;
	//if(abs(pp.x/pp.w)<1 && abs(pp.y/pp.w)<1 && abs(pp.z/pp.w)<1)
	//if(pp.w + sphereRadius < 0)
	//	return;
	if(dot(rightFacenormal,p.xyz) - rightFacedistance > sphereRadius)
		return;
	if(dot(leftFacenormal,p.xyz) - leftFacedistance > sphereRadius)
		return;
	if(dot(topFacenormal,p.xyz) - topFacedistance > sphereRadius)
		return;
	if(dot(bottomFacenormal,p.xyz) - bottomFacedistance > sphereRadius)
		return;
	if(dot(nearFacenormal,p.xyz) - nearFacedistance > sphereRadius)
		return;
	if(dot(farFacenormal,p.xyz) - farFacedistance > sphereRadius)
		return;
	currValidInstancePropsPoint[commands[idy].baseInstance + atomicAdd(commands[idy].instanceCount, 1)].point = float(bidx);
	//currValidInstancePropsPoint[bidx].point = float(bidx);
}
}