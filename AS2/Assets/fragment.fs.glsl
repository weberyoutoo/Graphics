#version 420 core

layout(location = 0) out vec4 fragColor;

uniform mat4 um4mv;
uniform mat4 um4p;

uniform vec3 LightPos;
uniform int bar;
uniform float time;
uniform int choice;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
    vec3 fragPos;
} vertexData;

layout (binding = 0) uniform sampler2D tex;
layout (binding = 1) uniform sampler2D noise;

float gaussian(float x, float sigma);


void main()
{
    vec4 texColor = texture(tex,vertexData.texcoord);

    vec3 ambient = 0.25 * vec3(1.0f, 1.0f, 1.0f);
    vec3 norm = normalize(vertexData.N);
    vec3 lightDir = normalize(LightPos - vertexData.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

	if(gl_FragCoord.x - bar < 0){
		if(abs(gl_FragCoord.x - bar) < 2)
			fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
		else if(texColor.a < 0.1)
			discard;
		else{
			vec4 result = vec4(ambient + diffuse, 1.0f) * texColor;
			fragColor = result;
		}
	}
	else{
		switch(choice){
			case 1:
				if(abs(gl_FragCoord.x - bar) < 2)
					fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
				else if(texColor.a < 0.1)
					discard;
				else{
					vec4 result = vec4(ambient + diffuse, 1.0f) * texColor;
					fragColor = result;
					}
				break;
			case 2:
				fragColor = vec4(norm, 1.0f);
				break;
			case 3:
				float radius1 = 2.0;
				float radius2 = 4.0;
				float quantizationLevel = 16;

				vec3 blurredColor1 = texture(tex, vertexData.texcoord + vec2(1.0) / textureSize(tex, 0) * radius1).rgb;
				vec3 blurredColor2 = texture(tex, vertexData.texcoord + vec2(1.0) / textureSize(tex, 0) * radius2).rgb;

				vec3 dog = blurredColor1 - blurredColor2;

				dog.r = (dog.r - min(min(dog.r, dog.g), dog.b)) / (max(max(dog.r, dog.g), dog.b) - min(min(dog.r, dog.g), dog.b));
				dog.g = dog.r;
				dog.b = dog.r;

				vec4 quantizedColor = floor(texColor * quantizationLevel) / quantizationLevel;
				fragColor = vec4(quantizedColor.rgb - dog*0.5, texColor.a);
				break;
			case 4:
				vec3 blurredColor = vec3(0.0f, 0.0f, 0.0f);
				for(int i = 0; i < 49; i++){
					vec2 offset = vec2(float(i % 7) - 3.0, float(i / 7) - 3.0) / textureSize(tex, 0);
					blurredColor += texture(tex, vertexData.texcoord + offset).rgb/49.0;
				}
				vec4 noiseColor = texture(noise,vertexData.texcoord);
				vec4 waterColor = vec4(0.8*blurredColor+0.2*noiseColor.rgb, texColor.a);

				waterColor = floor(waterColor * 8) / 8;
				fragColor = waterColor;
				break;
			case 5:
				float pixelsize = 50.0;
				vec2 pixelCoord = vec2(floor(vertexData.texcoord.x * 50), floor(vertexData.texcoord.y * 50));
				pixelCoord = pixelCoord/pixelsize;

				fragColor = texture(tex, pixelCoord);
				break;
			case 6:
				float offset = time;
				vec2 waveCoord = vertexData.texcoord;
				waveCoord.x += sin(vertexData.texcoord.y * 2 * 3.1415 + offset) * 0.01;
				fragColor = texture(tex,waveCoord);
				break;
		}
	}
	// diffuse color with compare bar-------------------------------------
	/*if(abs(gl_FragCoord.x - bar) < 2)
		fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	else if(texColor.a < 0.1)
		discard;
	else{
		vec4 result = vec4(ambient + diffuse, 1.0f) * texColor;
		fragColor = result;
	}*/

	// Output normal as color----------------------------------------------
	//fragColor = vec4(norm, 1.0f);

	// Image Abstraction-------------------------------------------------------------
	/*float radius1 = 2.0;
    float radius2 = 4.0;
		float quantizationLevel = 16;


    vec3 blurredColor1 = texture(tex, vertexData.texcoord + vec2(1.0) / textureSize(tex, 0) * radius1).rgb;
    vec3 blurredColor2 = texture(tex, vertexData.texcoord + vec2(1.0) / textureSize(tex, 0) * radius2).rgb;

    vec3 dog = blurredColor1 - blurredColor2;

    dog.r = (dog.r - min(min(dog.r, dog.g), dog.b)) / (max(max(dog.r, dog.g), dog.b) - min(min(dog.r, dog.g), dog.b));
    dog.g = dog.r;
    dog.b = dog.r;

	vec4 quantizedColor = floor(texColor * quantizationLevel) / quantizationLevel;
	fragColor = vec4(quantizedColor.rgb - dog*0.5, texColor.a);*/
	// 
	// waterColor
	/*vec3 blurredColor = vec3(0.0f, 0.0f, 0.0f);
		for(int i = 0; i < 49; i++){
			vec2 offset = vec2(float(i % 7) - 3.0, float(i / 7) - 3.0) / textureSize(tex, 0);
			blurredColor += texture(tex, vertexData.texcoord + offset).rgb/49.0;
		}
		vec4 noiseColor = texture(noise,vertexData.texcoord);
		vec4 waterColor = vec4(0.8*blurredColor+0.2*noiseColor.rgb, texColor.a);

		waterColor = floor(waterColor * 8) / 8;
		fragColor = waterColor;
	*/
	// pixel
	/*float pixelsize = 50.0;
        vec2 pixelCoord = vec2(floor(vertexData.texcoord.x * 50), floor(vertexData.texcoord.y * 50));
		pixelCoord = pixelCoord/pixelsize;

        fragColor = texture(tex, pixelCoord);
	*/
	// sine wave
	/*
	float offset = time;
		vec2 waveCoord = vertexData.texcoord;
		waveCoord.x += sin(vertexData.texcoord.y * 2 * 3.1415 + offset) * 0.01;
		fragColor = texture(tex,waveCoord);
	*/
}