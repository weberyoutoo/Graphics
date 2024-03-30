#version 430 core

layout (location = 0) out vec4 frag_color;

layout (location = 1) uniform sampler2D position_map;
layout (location = 2) uniform sampler2D normal_map;
layout (location = 3) uniform sampler2D diffuse_map;
layout (location = 4) uniform sampler2D specular_map;
layout (location = 5) uniform sampler2D ambient_map;

uniform int displayMode;

vec3 diffuse_albedo = vec3(0.64f);
vec3 specular_albedo = vec3(0.16f);
float specular_power = 1.0;
vec3 ambient_albedo = vec3(0.2f);

void main(void){
	vec4 result = vec4(0, 0, 0, 1);

	vec3 position = texelFetch(position_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 normal = texelFetch(normal_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 diffuse = texelFetch(diffuse_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 specular = texelFetch(specular_map, ivec2(gl_FragCoord.xy), 0).rgb;
	vec3 ambient = texelFetch(ambient_map, ivec2(gl_FragCoord.xy), 0).rgb;
	
	if(displayMode == 0 || displayMode == 6){
		vec3 N = (normal - 0.5) * 2.0;
		//vec3 N = (normalize(normal) - 0.5)*2.0;

		vec3 L = vec3(0.4, 0.5, 0.8);
		L = normalize(L);
		vec3 V = -position;
		V = normalize(V);
		vec3 H = normalize(L + V);

		if(specular == vec3(1.0))
			specular_power = 32.0;
	
		float NdotL = max(0.0, dot(N, L));                                       
		float NdotH = max(0.0, dot(N, H));   
	
		vec3 ambient_color  = ambient_albedo * diffuse;                  
		vec3 diffuse_color  = diffuse_albedo * NdotL * diffuse;                  
		vec3 specular_color =  specular_albedo * pow(NdotH, specular_power) * specular;                 
		result += vec4((ambient_color + diffuse_color + specular_color), 0.0);
	
		frag_color.rgb = pow(result.rgb, vec3(0.5));
		frag_color.a = result.a;
	}
	else if(displayMode == 1)
		frag_color = vec4(position, 1.0);
	else if(displayMode == 2)
		frag_color = vec4(normal, 1.0);
	else if(displayMode == 3)
		frag_color = vec4(diffuse, 1.0);	
	else if(displayMode == 4)
		frag_color = vec4(specular, 1.0);
	else
		frag_color = vec4(diffuse, 1.0);
	//frag_color = vec4(normal, 1.0f);
}
