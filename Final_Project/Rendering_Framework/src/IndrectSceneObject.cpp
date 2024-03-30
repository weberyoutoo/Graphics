#include "IndrectSceneObject.h"


IndrectSceneObject::IndrectSceneObject(ShaderProgram* shaderProgram,ShaderProgram* computeshaderProgram, ShaderProgram* resetcomputeshaderProgram)
{
	m_shaderProgram = shaderProgram;
	m_computeshaderProgram = computeshaderProgram;
	m_resetcomputeshaderProgram = resetcomputeshaderProgram;
	std::cout << "indrect ready \n";
	this->scene[0] = aiImportFile("assets\\grassB.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	this->scene[1] = aiImportFile("assets\\bush01_lod2.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	this->scene[2] = aiImportFile("assets\\bush05_lod2.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	this->scene[3] = aiImportFile("assets\\Medieval_Building_LowPoly\\medieval_building_lowpoly_2.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	this->scene[4] = aiImportFile("assets\\Medieval_Building_LowPoly\\medieval_building_lowpoly_1.obj", aiProcessPreset_TargetRealtime_MaxQuality);
	//texture
	const int NUM_TEXTURE = 5;
	const int IMG_WIDTH = 1024;
	const int IMG_HEIGHT = 1024;
	const int IMG_CHANNEL = 4;
	glGenTextures(1, &diffuse_tex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse_tex);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, IMG_WIDTH, IMG_HEIGHT, NUM_TEXTURE);
	char texturepath[5][100];
	strcpy_s(texturepath[0], "assets\\grassB_albedo.png");
	strcpy_s(texturepath[1], "assets\\bush01.png");
	strcpy_s(texturepath[2], "assets\\bush05.png");
	strcpy_s(texturepath[3], "assets\\Medieval_Building_LowPoly\\Medieval_Building_LowPoly_V2_Albedo_small.png");
	strcpy_s(texturepath[4], "assets\\Medieval_Building_LowPoly\\Medieval_Building_LowPoly_V1_Albedo_small.png");
	for (unsigned int i = 0; i < 5; ++i)
	{
		texture_data texture = loadImg(texturepath[i]);
		std::cout << texturepath[i] << std::endl;
		GLsizei width = texture.width;
		GLsizei height = texture.height;
		unsigned char* data = texture.data;
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, IMG_WIDTH, IMG_HEIGHT, 1, GL_RGBA,
			GL_UNSIGNED_BYTE, data);
	}
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// IMPORTANT !! Use mipmap for the foliage rendering
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// create 3 vbos to hold data
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &cmdBufferHandle);
	glGenBuffers(1, &OffsetsBufferHandle);
	glGenBuffers(1, &PointBufferHandleValid);
	int IdicesShift = 0;
	int CountShift = 0;
	for (unsigned int i = 0; i < 5; ++i)
	{
		const aiScene* scene_grassB = this->scene[i];
		aiMesh* mesh = scene_grassB->mMeshes[0];
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			// mesh->mVertices[v][0~2] => position
			// mesh->mNormals[v][0~2] => normal
			// mesh->mTextureCoords[0][v][0~1] => texcoord
			grassBvertices.push_back(mesh->mVertices[v][0]);
			grassBvertices.push_back(mesh->mVertices[v][1]);
			grassBvertices.push_back(mesh->mVertices[v][2]);
			//cout << mesh->mVertices[v][0] << " " << mesh->mVertices[v][1] << " " << mesh->mVertices[v][2] << "\n";
			grassBnormals.push_back(mesh->mNormals[v][0]);
			grassBnormals.push_back(mesh->mNormals[v][1]);
			grassBnormals.push_back(mesh->mNormals[v][2]);
			grassBtexcoords.push_back(mesh->mTextureCoords[0][v][0]);
			grassBtexcoords.push_back(mesh->mTextureCoords[0][v][1]);
			grassBtexcoords.push_back(i);
			//cout << mesh->mTextureCoords[0][v][0] << " " << mesh->mTextureCoords[0][v][1] << "\n";
		}

		for (unsigned int f = 0; f < mesh->mNumFaces; ++f)
		{
			// mesh->mFaces[f].mIndices[0~2] => index
			grassBindices.push_back(mesh->mFaces[f].mIndices[0]);
			grassBindices.push_back(mesh->mFaces[f].mIndices[1]);
			grassBindices.push_back(mesh->mFaces[f].mIndices[2]);
			//cout << mesh->mFaces[f].mIndices[0] << " " << mesh->mFaces[f].mIndices[1] << " " << mesh->mFaces[f].mIndices[2] << '\n';
		}
		// glVertexAttribPointer / glEnableVertexArray calls¡K
		//cout << shape.materialID << endl;
		drawcommand[i].count = mesh->mNumFaces * 3;
		drawCount[i] = mesh->mNumFaces * 3;
		drawcommand[i].baseVertex = IdicesShift;
		IdicesShift = IdicesShift + mesh->mNumVertices;
		drawcommand[i].firstIndex = CountShift;
		indices[i] = (GLvoid*)(CountShift * 4);
		CountShift = CountShift + drawCount[i];

		aiReleaseImport(scene_grassB);

	}
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, grassBvertices.size() * sizeof(float) + grassBtexcoords.size() * sizeof(float) + grassBnormals.size() * sizeof(float), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, grassBvertices.size() * sizeof(float), grassBvertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, grassBvertices.size() * sizeof(float), grassBtexcoords.size() * sizeof(float), grassBtexcoords.data());
	glBufferSubData(GL_ARRAY_BUFFER, grassBvertices.size() * sizeof(float) + grassBtexcoords.size() * sizeof(float), grassBnormals.size() * sizeof(float), grassBnormals.data());

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(grassBvertices.size() * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(grassBvertices.size() * sizeof(float) + grassBtexcoords.size() * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, grassBindices.size() * sizeof(int), grassBindices.data(), GL_STATIC_DRAW);

	MyPoissonSample* sample[5];
	sample[0] = MyPoissonSample::fromFile("assets\\poissonPoints_621043_after.ppd2");
	sample[1] = MyPoissonSample::fromFile("assets\\poissonPoints_1010.ppd2");
	sample[2] = MyPoissonSample::fromFile("assets\\poissonPoints_2797.ppd2");
	sample[3] = MyPoissonSample::fromFile("assets\\cityLots_sub_0.ppd2");
	sample[4] = MyPoissonSample::fromFile("assets\\cityLots_sub_1.ppd2");
	int point = 0;
	for (unsigned int i = 0; i < 5; ++i)
	{
		int NUM_SAMPLE = sample[i]->m_numSample;
		//if (i == 0)
		//	NUM_SAMPLE = 100;
		std::cout << NUM_SAMPLE << std::endl;
		for (int idx = 0; idx < NUM_SAMPLE; idx++) {
			glm::vec3 position(

				sample[i]->m_positions[idx * 3 + 0], sample[i]->m_positions[idx * 3 + 1], sample[i]->m_positions[idx * 3 + 2]

			);
			glm::vec3 rad(

				sample[i]->m_radians[idx * 3 + 0], sample[i]->m_radians[idx * 3 + 1], sample[i]->m_radians[idx * 3 + 2]

			);

			// calculate rotation matrix

			glm::quat q = glm::quat(rad);

			glm::mat4 rotationMatrix = glm::toMat4(q);
			glm::mat4 t_model = glm::translate(glm::mat4(1.0), position);
			grassOffsets.push_back(t_model* rotationMatrix);
			/*if (i == 0)
			{
				grassOffsets.push_back(0);
				grassOffsets.push_back(idx);
				grassOffsets.push_back(0);
				grassOffsets.push_back(0.0);
			}
			else
			{
				grassOffsets.push_back(sample[i]->m_positions[idx * 3 + 0]);
				grassOffsets.push_back(sample[i]->m_positions[idx * 3 + 1]);
				grassOffsets.push_back(sample[i]->m_positions[idx * 3 + 2]);
				grassOffsets.push_back(0.0);
			}*/
			
			grassPoint.push_back(point);
			point += 1;
		}
		if (i == 0)
		{
			drawcommand[0].baseInstance = 0;
			drawcommand[0].instanceCount = NUM_SAMPLE;
		}
		else
		{
			drawcommand[i].baseInstance = drawcommand[i-1].baseInstance + drawcommand[i-1].instanceCount;
			drawcommand[i].instanceCount = NUM_SAMPLE;
		}
	}
	drawcommand[5].baseInstance = drawcommand[4].baseInstance + drawcommand[4].instanceCount;

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, OffsetsBufferHandle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, grassOffsets.size() * sizeof(glm::mat4), grassOffsets.data(), GL_MAP_READ_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, OffsetsBufferHandle);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, PointBufferHandleValid);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, grassPoint.size() * sizeof(float), grassPoint.data(), GL_MAP_READ_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, PointBufferHandleValid);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, PointBufferHandleValid);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cmdBufferHandle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(drawcommand), drawcommand, GL_MAP_READ_BIT);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cmdBufferHandle);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);

}


IndrectSceneObject::~IndrectSceneObject()
{

}
void IndrectSceneObject::shadow_update() {
	//m_shaderProgram->useProgram();
	glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, 12);
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)((sizeof(DrawCommand)) * startdrawnum), drawnum, 0);
}
void IndrectSceneObject::update() {
	//m_shaderProgram->useProgram();
	glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, 12);
	glBindVertexArray(vao);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D_ARRAY, diffuse_tex);
	glUniform1i(glGetUniformLocation(m_shaderProgram->programId(), "tex"), 5);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, cmdBufferHandle);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)((sizeof(DrawCommand)) * startdrawnum), drawnum, 0);
}

void IndrectSceneObject::compute_collect(glm::mat4 vp, Frustum playerfrustum) {
	m_resetcomputeshaderProgram->useProgram();
	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	m_computeshaderProgram->useProgram();
	glUniformMatrix4fv(0, 1, false, glm::value_ptr(vp));
	glUniform3fv(4, 1, glm::value_ptr(playerfrustum.rightFace.normal));
	glUniform1f(5, playerfrustum.rightFace.distance);
	glUniform3fv(6, 1, glm::value_ptr(playerfrustum.leftFace.normal));
	glUniform1f(7, playerfrustum.leftFace.distance);
	glUniform3fv(8, 1, glm::value_ptr(playerfrustum.topFace.normal));
	glUniform1f(9, playerfrustum.topFace.distance);
	glUniform3fv(10, 1, glm::value_ptr(playerfrustum.bottomFace.normal));
	glUniform1f(11, playerfrustum.bottomFace.distance);
	glUniform3fv(12, 1, glm::value_ptr(playerfrustum.nearFace.normal));
	glUniform1f(13, playerfrustum.nearFace.distance);
	glUniform3fv(14, 1, glm::value_ptr(playerfrustum.farFace.normal));
	glUniform1f(15, playerfrustum.farFace.distance);
	glUniform1i(16, drawcommand[startdrawnum].baseInstance);

	glDispatchCompute((drawcommand[startdrawnum + drawnum].baseInstance - drawcommand[startdrawnum].baseInstance) / 512 + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void IndrectSceneObject::setdrawnum(int st, int num) {
	startdrawnum = st;
	drawnum = num;
}