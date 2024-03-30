#version 420 core

layout(location = 0) in vec3 iv3vertex;
layout(location = 1) in vec2 iv2tex_coord;
layout(location = 2) in vec3 iv3normal;

uniform mat4 um4mv;
uniform mat4 um4p;

out VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
    vec3 fragPos;
} vertexData;

void main()
{   
    mat3 normalMatrix = mat3(um4mv);
    normalMatrix = inverse(normalMatrix);
    normalMatrix = transpose(normalMatrix);

	gl_Position = um4p * um4mv * vec4(iv3vertex, 1.0);

    vertexData.texcoord = iv2tex_coord;
    vertexData.N = normalMatrix * iv3normal;
    vertexData.fragPos = vec3(um4mv * vec4(iv3vertex, 1.0));
}