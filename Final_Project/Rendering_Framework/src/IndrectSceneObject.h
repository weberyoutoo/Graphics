#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "SceneManager.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "stb_image.h"
#include "MyPoissonSample.h"
#include <iostream>
#include <vector>
#include "Shader.h"
#include "MyCameraManager.h"

typedef struct _texture_data
{
	_texture_data() : width(0), height(0), data(0) {}
	int width;
	int height;
	unsigned char* data;
} texture_data;

struct DrawCommand {
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLuint baseVertex;
	GLuint baseInstance;
};


class IndrectSceneObject 
{

private:
	const aiScene* scene[5];
	GLuint diffuse_tex;
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	GLuint cmdBufferHandle;
	GLuint OffsetsBufferHandle;
	GLuint PointBufferHandleValid;
	std::vector<float> grassBvertices, grassBtexcoords, grassBnormals;
	std::vector<int> grassBindices;
	int drawCount[5];
	GLvoid* indices[5];
	DrawCommand drawcommand[6];
	std::vector<glm::mat4> grassOffsets;
	std::vector<float> grassPoint;
	ShaderProgram* m_shaderProgram;
	ShaderProgram* m_computeshaderProgram;
	ShaderProgram* m_resetcomputeshaderProgram;
	int startdrawnum = 3;
	int drawnum = 2;
public:
	IndrectSceneObject(ShaderProgram* shaderProgram,ShaderProgram* computeshaderProgram, ShaderProgram* resetcomputeshaderProgram);
	virtual ~IndrectSceneObject();
	texture_data loadImg(const char* path)
	{
		texture_data texture;
		int n;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* data = stbi_load(path, &texture.width, &texture.height, &n, 4);
		if (data != NULL)
		{
			texture.data = new unsigned char[texture.width * texture.height * 4 * sizeof(unsigned char)];
			memcpy(texture.data, data, texture.width * texture.height * 4 * sizeof(unsigned char));
			stbi_image_free(data);
		}
		return texture;
	}
	void setdrawnum(int st, int num);
	void update();
	void shadow_update();
	void compute_collect(glm::mat4 mv, Frustum playerfrustum);
};

