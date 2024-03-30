#version 430 core
struct InstanceProperties{
mat4 position ;
};

layout(location=0) in vec3 v_vertex;
layout(location=1) in vec3 v_normal ;
layout(location=2) in vec3 v_uv ;
layout(location = 3) in float point;

layout (std430, binding=1) buffer InstanceData{
InstanceProperties instanceProps[] ;
};

out vec3 f_viewVertex ;
out vec3 f_uv ;
out vec3 f_worldVertex;
out vec3 f_normal;

out vec4 pp1 ;
out vec4 pp2 ;
out vec4 pp3 ;

layout(location = 0) uniform mat4 modelMat ;
layout(location = 5) uniform sampler2D elevationMap ;
layout(location = 6) uniform sampler2D normalMap ;
layout(location = 7) uniform mat4 viewMat ;
layout(location = 8) uniform mat4 projMat ;
layout(location = 9) uniform mat4 terrainVToUVMat;
layout(location = 1) uniform int vertexProcessIdx ;

uniform int airplane;

layout(location = 10) uniform mat4 L1MV ;
layout(location = 11) uniform mat4 L2MV ;
layout(location = 12) uniform mat4 L3MV ;


void commonProcess(){
	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0) ;
	vec4 worldNormal = modelMat * vec4(v_normal, 0.0) ;

	if (vertexProcessIdx == 13 && airplane == 0)
		worldVertex = vec4(v_vertex, 1.0) + vec4(25.92, 18.27, 11.75, 0.0);


	vec4 viewVertex = viewMat * worldVertex ;
	vec4 viewNormal = viewMat * worldNormal ;
	
	f_viewVertex = viewVertex.xyz;
	f_uv = v_uv ;
	f_worldVertex = normalize(worldVertex.xyz) * 0.5 + 0.5;
	f_normal = normalize(worldNormal.xyz) * 0.5 + 0.5;

	gl_Position = projMat * viewVertex ;

	pp1 = L1MV*worldVertex;
	pp2 = L2MV*worldVertex;
	pp3 = L3MV*worldVertex;
}

void indrectProcess(){
	vec4 worldVertex = instanceProps[int(point)].position * vec4(v_vertex, 1.0);
	//vec4 worldNormal = modelMat * vec4(v_normal, 0.0) ;
	vec4 worldNormal = instanceProps[int(point)].position * vec4(v_normal, 0.0) ;

	vec4 viewVertex = viewMat * worldVertex ;
	vec4 viewNormal = viewMat * worldNormal ;

	gl_Position = projMat * viewVertex;
	f_uv = v_uv;
	f_viewVertex = viewVertex.xyz;
	f_worldVertex = normalize(worldVertex.xyz) * 0.5 + 0.5;
	f_normal = normalize(worldNormal.xyz) * 0.5 + 0.5;

	pp1 = L1MV*worldVertex;
	pp2 = L2MV*worldVertex;
	pp3 = L3MV*worldVertex;
}

void terrainProcess(){
	vec4 worldV = modelMat * vec4(v_vertex, 1.0) ;
	// calculate uv
	vec4 uv = terrainVToUVMat * worldV ;
	uv.y = uv.z ;
	// get height from map
	float h = texture(elevationMap, uv.xy).r ;
	worldV.y = h;		
	// get normal from map
	vec4 normalTex = texture(normalMap, uv.xy) ;
	// [0, 1] -> [-1, 1]
	normalTex = normalTex * 2.0 - 1.0 ;
		
	// transformation	
	vec4 viewVertex = viewMat * worldV ;
	vec4 viewNormal = viewMat * vec4(normalTex.rgb, 0) ;	
	
	f_viewVertex = viewVertex.xyz;
	f_uv = uv.xyz ;
	f_worldVertex = normalize(worldV.xyz) * 0.5 + 0.5;
	f_normal = normalize(normalTex.xyz) * 0.5 + 0.5;

	gl_Position = projMat * viewVertex ;

	pp1 = L1MV*worldV;
	pp2 = L2MV*worldV;
	pp3 = L3MV*worldV;
}

void main(){
	if(vertexProcessIdx == 0){
		commonProcess() ;
	}
	else if(vertexProcessIdx == 12){
		indrectProcess() ;
	}
	else if(vertexProcessIdx == 3){
		terrainProcess() ;
	}
	else{
		commonProcess() ;
	}	
}