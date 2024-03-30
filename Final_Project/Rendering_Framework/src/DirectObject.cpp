#include "DirectObject.h"

#include "stb_image.h"

DirectObject::DirectObject(ShaderProgram *program) {
	programID = program->programId();

	loadModel("assets\\airplane.obj", 0);
	loadModel("assets\\MagicRock\\magicRock.obj", 0);

	textureInit();

	loadTextureFile("assets\\Airplane_smooth_DefaultMaterial_BaseMap.jpg", modelTex);
	loadTextureFile("assets\\MagicRock\\StylMagicRocks_AlbedoTransparency.png", modelTex2);
	loadTextureFile("assets\\MagicRock\\StylMagicRocks_NormalOpenGL.png", normalMap2);

}
DirectObject::~DirectObject(){}

void DirectObject::update() {
	Draw(programID);
}

void DirectObject::Draw(GLuint program)
{
	glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, 13);
	glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, 13);

	glActiveTexture(GL_TEXTURE6);
	glUniform1i(glGetUniformLocation(program, "modelTex"), 6);
	glBindTexture(GL_TEXTURE_2D_ARRAY, modelTex);
	glActiveTexture(GL_TEXTURE7);
	glUniform1i(glGetUniformLocation(program, "modelTex2"), 7);//modelTex2
	glBindTexture(GL_TEXTURE_2D_ARRAY, modelTex2);
	glActiveTexture(GL_TEXTURE8);
	glUniform1i(glGetUniformLocation(program, "normalMap2"), 8);//normalMap2
	glBindTexture(GL_TEXTURE_2D_ARRAY, normalMap2);

	glUniform1i(glGetUniformLocation(program, "airplane"), 0);

	glBindVertexArray(meshes[1].VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(meshes[1].indices.size()), GL_UNSIGNED_INT, 0);

	glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, value_ptr(this->airplaneMat));

	glUniform1i(glGetUniformLocation(program, "airplane"), 1);
	glUniform1i(glGetUniformLocation(program, "dis_normalMap"), normalMap_flag);

	glBindVertexArray(meshes[0].VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(meshes[0].indices.size()), GL_UNSIGNED_INT, 0);

}

void DirectObject::shadow_Draw(GLuint program) {
	glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, 13);
	glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, 13);
	glUniform1i(glGetUniformLocation(program, "airplane"), 0);

	glBindVertexArray(meshes[1].VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(meshes[1].indices.size()), GL_UNSIGNED_INT, 0);

	glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, value_ptr(this->airplaneMat));

	glUniform1i(glGetUniformLocation(program, "airplane"), 1);

	glBindVertexArray(meshes[0].VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(meshes[0].indices.size()), GL_UNSIGNED_INT, 0);
}

Mesh DirectObject::processMesh(aiMesh *mesh, const aiScene *scene, const int layer)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vec3 vector;
		vec3 vec;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		vec.x = mesh->mTextureCoords[0][i].x;
		vec.y = mesh->mTextureCoords[0][i].y;
		vec.z = 0;
		vertex.TexCoords = vec;

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	vertexCount += mesh->mNumVertices;
	//aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

	// Load material textures manually (you can replace this part)

	return Mesh(vertices, indices, layer, mesh->mNumVertices);
}

void DirectObject::processNode(aiNode *node, const aiScene *scene, int layer)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene, layer));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene, layer);
	}
}

void DirectObject::loadModel(string const &path, int layer)
{
	Assimp::Importer importer;
	//const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	const aiScene *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR:" << importer.GetErrorString() << endl;
		return;
	}

	processNode(scene->mRootNode, scene, layer);

	cout << "Load " << vertexCount << " vertices" << endl;
	vertexCount = 0;
}

void DirectObject::textureInit()
{
	glGenTextures(1, &modelTex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, modelTex);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, 1024, 1024, 1);

	glGenTextures(1, &modelTex2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, modelTex2);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, 2048, 2048, 1);

	glGenTextures(1, &normalMap2);
	glBindTexture(GL_TEXTURE_2D_ARRAY, normalMap2);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, 2048, 2048, 1);
	
}

void DirectObject::loadTextureFile(const char *path, GLuint tex)
{
	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 4);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		cout << path << endl;
		//cout << width << height << endl;
	}
	else
	{
		cout << "Texture failed to load at path: " << path << endl;
		stbi_image_free(data);
	}
	
}

void DirectObject::setAirplane(mat4 aMat) {
	this->airplaneMat = aMat;
}