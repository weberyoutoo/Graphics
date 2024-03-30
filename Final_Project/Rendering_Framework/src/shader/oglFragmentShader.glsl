#version 430 core

layout (location = 4) out vec4 frag_position;
layout (location = 1) out vec4 frag_normal;
layout (location = 2) out vec4 frag_diffuse;
layout (location = 3) out vec4 frag_specular;
layout (location = 0) out vec4 frag_ambient;

in vec3 f_viewVertex ;
in vec3 f_uv ;
in vec3 f_worldVertex;
in vec3 f_normal;

in vec4 pp1 ;
in vec4 pp2 ;
in vec4 pp3 ;
in vec4 ppt1 ;
in vec4 ppt2 ;
in vec4 ppt3 ;

//layout (location = 0) out vec4 fragColor ;

layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;

uniform sampler2DArray tex;
uniform sampler2DArray modelTex;
uniform sampler2DArray modelTex2;
uniform sampler2DArray normalMap2;

uniform int airplane;
uniform bool dis_normalMap;

uniform sampler2DShadow shadow_tex;
uniform sampler2DShadow shadow_tex2;
uniform sampler2DShadow shadow_tex3;

layout(location = 13) uniform float mp;
float fp = 0.0;

vec4 withFog(vec4 color){
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1) ;
	const float MAX_DIST = 400.0 ;
	const float MIN_DIST = 350.0 ;
	
	float dis = length(f_viewVertex) ;
	float fogFactor = (MAX_DIST - dis) / (MAX_DIST - MIN_DIST) ;
	fogFactor = clamp(fogFactor, 0.0, 1.0) ;
	fogFactor = fogFactor * fogFactor ;
	
	vec4 colorWithFog = mix(FOG_COLOR, color, fogFactor) ;
	return colorWithFog ;
}


void terrainPass(){
	vec4 texel = texture(albedoTexture, f_uv.xy) ;
	float shadow_factor = 1.0;  
	if(abs(pp1.x/pp1.w)<1 && abs(pp1.y/pp1.w)<1 )
	{
		texel = texel*(1-mp) + vec4(1.0, 0.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex, ppt1);  
	}
	else if(abs(pp2.x/pp2.w)<1 && abs(pp2.y/pp2.w)<1 )
	{
		texel = texel*(1-mp) + vec4(0.0, 1.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex2, ppt2);  
	}
	else if(abs(pp3.x/pp3.w)<1 && abs(pp3.y/pp3.w)<1)
	{
		texel = texel*(1-mp) + vec4(0.0, 0.0, 1.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex3, ppt3); 
	}
	shadow_factor =  (shadow_factor+fp)/(fp+1);
	frag_position = withFog(vec4(f_worldVertex, 1.0f));
	frag_normal = withFog(vec4(f_normal, 1.0f));
	frag_diffuse = withFog(texel)*shadow_factor;
	frag_specular = withFog(vec4(0.0f))*shadow_factor;
	frag_ambient = withFog(texel);
}

void pureColor(){
	vec4 Color = withFog(vec4(1.0, 0.0, 0.0, 1.0)) ;
	float shadow_factor = 1.0;  
	if(abs(pp1.x/pp1.w)<1 && abs(pp1.y/pp1.w)<1)
	{
		Color = Color*(1-mp) + vec4(1.0, 0.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex, ppt1); 
	}
	else if(abs(pp2.x/pp2.w)<1 && abs(pp2.y/pp2.w)<1 )
	{
		Color = Color*(1-mp) + vec4(0.0, 1.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex2, ppt2); 
	}
	else if(abs(pp3.x/pp3.w)<1 && abs(pp3.y/pp3.w)<1)
	{
		Color = Color*(1-mp) + vec4(0.0, 0.0, 1.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex3, ppt3); 
	}
	shadow_factor =  (shadow_factor+fp)/(fp+1);
	frag_position = vec4(f_worldVertex, 1.0f);
	frag_normal = vec4(normalize(f_normal), 1.0f);
	frag_diffuse = Color*shadow_factor;
	frag_specular = vec4(0.0f)*shadow_factor;
	frag_ambient = Color;
}

void indrectColor(){
	vec4 Color = texture(tex, f_uv) ;
	float shadow_factor = 1.0;  
	if(abs(pp1.x/pp1.w)<1 && abs(pp1.y/pp1.w)<1)
	{
		Color = Color*(1-mp) + vec4(1.0, 0.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex, ppt1); 
	}
	else if(abs(pp2.x/pp2.w)<1 && abs(pp2.y/pp2.w)<1 )
	{
		Color = Color*(1-mp) + vec4(0.0, 1.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex2, ppt2); 
	}
	else if(abs(pp3.x/pp3.w)<1 && abs(pp3.y/pp3.w)<1)
	{
		Color = Color*(1-mp) + vec4(0.0, 0.0, 1.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex3, ppt3); 
	}
	shadow_factor =  (shadow_factor+fp)/(fp+1);
	if (Color.a<=0.5)
		discard;
	else{
		//vec4 Color = vec4(f_normal, 1.0f);
		frag_position = withFog(vec4(f_worldVertex, 1.0f));
		frag_normal = withFog(vec4(f_normal, 1.0f));
		frag_diffuse = withFog(Color)*shadow_factor;
		frag_specular = withFog(vec4(0.0f))*shadow_factor;
		frag_ambient = withFog(Color);
	}
}

void directColor(){
	vec4 Color = vec4(0.0);
	if(airplane == 1)
		Color = texture(modelTex, f_uv);
	else
		Color = texture(modelTex2, f_uv);		
	float shadow_factor = 1.0;  
	if(abs(pp1.x/pp1.w)<1 && abs(pp1.y/pp1.w)<1 )
	{
		Color = Color*(1-mp) + vec4(1.0, 0.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex, ppt1); 
	}
	else if(abs(pp2.x/pp2.w)<1 && abs(pp2.y/pp2.w)<1 )
	{
		Color = Color*(1-mp) + vec4(0.0, 1.0, 0.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex2, ppt2); 
	}
	else if(abs(pp3.x/pp3.w)<1 && abs(pp3.y/pp3.w)<1)
	{
		Color = Color*(1-mp) + vec4(0.0, 0.0, 1.0, 1.0)*mp;
		shadow_factor = textureProj(shadow_tex3, ppt3); 
	}
	shadow_factor =  (shadow_factor+fp)/(fp+1);
	frag_position = withFog(vec4(f_worldVertex, 1.0f));
	frag_normal = withFog(vec4(f_normal, 1.0f));	
	if(airplane == 0 && dis_normalMap == false){
		vec4 normalTex = texture(normalMap2, f_uv);
		normalTex = vec4(normalTex.rgb, 1.0);
		frag_normal = withFog(normalTex);
	}
	frag_diffuse = withFog(Color)*shadow_factor;
	frag_specular = withFog(vec4(1.0f))*shadow_factor;
	frag_ambient = withFog(Color);
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