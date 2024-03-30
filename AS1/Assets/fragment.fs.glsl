#version 410

layout(location = 0) out vec4 fragColor;

uniform mat4 um4mv;
uniform mat4 um4p;

uniform vec3 LightPos;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
    vec3 fragPos;
} vertexData;

uniform sampler2D tex;

void main()
{
    vec3 texColor = texture(tex,vertexData.texcoord).rgb;
    //fragColor = vec4(texColor, 1.0);
    //vec3 objectColor = vec3(0.0f, 0.0f, 1.0f);
    vec3 ambient = 0.25 * vec3(1.0f, 1.0f, 1.0f);

    vec3 norm = normalize(vertexData.N);
    vec3 lightDir = normalize(LightPos - vertexData.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);
    
    //vec3 result = (ambient + diffuse) * objectColor;
    vec3 result = (ambient + diffuse) * texColor;
    fragColor = vec4(result, 1.0f);
}