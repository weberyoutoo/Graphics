#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include "stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#define uchar unsigned char
#include "SceneManager.h"
#include "Shader.h"
#include "MyCameraManager.h"


using namespace std;
using namespace glm;

struct Vertex {
	vec3 Position;
	vec3 TexCoords;
	vec3 Normal;
};


class Mesh {
public:
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	int layer;
	GLuint verticeCount;


	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, const int layer, int verticesCount)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->layer = layer;
		this->verticeCount = verticesCount;

		cout << vertices.size() << indices.size() << endl;

		setupMesh();
	}

private:
	void setupMesh() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
	}

};

class DirectObject 
{
private:
	GLuint programID;
	GLuint modelTex;
	GLuint modelTex2;
	GLuint normalMap2;
	vector<Mesh> meshes;
	int vertexCount = 0;
	bool normalMap_flag = false;

	mat4 airplaneMat;

public:
	DirectObject(ShaderProgram *program);
	virtual ~DirectObject();

	void update();
	void Draw(GLuint program);
	void shadow_Draw(GLuint program);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene, const int layer);
	void processNode(aiNode *node, const aiScene *scene, int layer);
	void loadModel(string const &path, int layer);
	void textureInit();
	void loadTextureFile(const char *path, GLuint tex);
	void setAirplane(mat4 aMat);
	void setNormalMapFlag(bool flag) { this->normalMap_flag = flag; };

};