#version 430 core

in vec3 f_viewVertex ;
in vec3 f_uv ;
in vec3 f_worldVertex;
in vec3 f_normal;

in vec4 pp1 ;
in vec4 pp2 ;
in vec4 pp3 ;

layout (location = 0) out vec4 fragColor ;

layout(location = 2) uniform int pixelProcessId;

uniform int airplane;



void terrainPass(){
	fragColor = vec4(f_normal,1.0);
}

void pureColor(){
	fragColor = vec4(f_normal,1.0);
}

void indrectColor(){
	fragColor = vec4(f_normal,1.0);

}

void directColor(){
	fragColor = vec4(f_normal,1.0);
}

void main(){	
	if(pixelProcessId == 5){
		pureColor() ;
	}
	else if(pixelProcessId == 12){
		indrectColor() ;
	}
	else if(pixelProcessId == 13){
		directColor();
	}
	else if(pixelProcessId == 7){
		terrainPass() ;
	}
	else{
		pureColor() ;
	}
}