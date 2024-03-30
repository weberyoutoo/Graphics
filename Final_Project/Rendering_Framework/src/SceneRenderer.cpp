#include "SceneRenderer.h"


SceneRenderer::SceneRenderer()
{
}


SceneRenderer::~SceneRenderer()
{
}
void SceneRenderer::startNewFrame() {
	this->m_shaderProgram->useProgram();
	this->clear();

}
void SceneRenderer::renderPass(){
	//glEnable(GL_DEPTH_TEST);
	SceneManager *manager = SceneManager::Instance();	
	glm::mat4 scale_bias_matrix = glm::translate(glm::mat4(1.0f), vec3(0.5f, 0.5f, 0.5f)) * glm::scale(glm::mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
	glUniformMatrix4fv(manager->m_projMatHandle, 1, false, glm::value_ptr(this->m_projMat));
	glUniformMatrix4fv(manager->m_viewMatHandle, 1, false, glm::value_ptr(this->m_viewMat));
	glUniformMatrix4fv(10, 1, false, glm::value_ptr(this->m_L1projMat * this->m_L1viewMat));
	glUniformMatrix4fv(11, 1, false, glm::value_ptr(this->m_L2projMat * this->m_L2viewMat));
	glUniformMatrix4fv(12, 1, false, glm::value_ptr(this->m_L3projMat * this->m_L3viewMat));
	glUniform1f(13, mp);
	glUniformMatrix4fv(14, 1, false, glm::value_ptr(scale_bias_matrix));

	if (this->m_terrainSO != nullptr) {
		glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, SceneManager::Instance()->m_vs_terrainProcess);
		this->m_terrainSO->update();
	}

	if (this->m_dynamicSOs.size() > 0) {
		glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, SceneManager::Instance()->m_vs_commonProcess);
		for (DynamicSceneObject *obj : this->m_dynamicSOs) {
			obj->update();
		}
	}
	
	if (this->m_indrectSO != nullptr) {
		glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, 12);
		this->m_indrectSO->update();
	}
	if (this->directObject != nullptr) {
		glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, 12);
		this->directObject->update();
	}
}

// =======================================
void SceneRenderer::resize(const int w, const int h){
	this->m_frameWidth = w;
	this->m_frameHeight = h;
}
bool SceneRenderer::initialize(const int w, const int h, ShaderProgram* shaderProgram){
	this->m_shaderProgram = shaderProgram;

	this->resize(w, h);
	const bool flag = this->setUpShader();
	
	if (!flag) {
		return false;
	}	
	
	glEnable(GL_DEPTH_TEST);

	return true;
}
void SceneRenderer::setProjection(const glm::mat4 &proj){
	this->m_projMat = proj;
}
void SceneRenderer::setView(const glm::mat4 &view){
	this->m_viewMat = view;
}
void SceneRenderer::setLProjection(const glm::mat4& proj1, const glm::mat4& proj2, const glm::mat4& proj3) {
	this->m_L1projMat = proj1;
	this->m_L2projMat = proj2;
	this->m_L3projMat = proj3;
}
void SceneRenderer::setLView(const glm::mat4& view1, const glm::mat4& view2, const glm::mat4& view3) {
	this->m_L1viewMat = view1;
	this->m_L2viewMat = view2;
	this->m_L3viewMat = view3;
}
void SceneRenderer::setViewport(const int x, const int y, const int w, const int h) {
	glViewport(x, y, w, h);
}
void SceneRenderer::appendDynamicSceneObject(DynamicSceneObject *obj){
	this->m_dynamicSOs.push_back(obj);
}
void SceneRenderer::appendTerrainSceneObject(TerrainSceneObject* tSO) {
	this->m_terrainSO = tSO;
}
void SceneRenderer::appendIndrectSceneObject(IndrectSceneObject* iSO) {
	this->m_indrectSO = iSO;
}
void SceneRenderer::appendDircectObject(DirectObject* dO) {
	this->directObject = dO;
}
void SceneRenderer::clear(const glm::vec4 &clearColor, const float depth){
	static const float COLOR[] = { 0.0, 0.0, 0.0, 1.0 };
	static const float DEPTH[] = { 1.0 };

	glClearBufferfv(GL_COLOR, 0, COLOR);
	glClearBufferfv(GL_DEPTH, 0, DEPTH);
}
bool SceneRenderer::setUpShader(){
	if (this->m_shaderProgram == nullptr) {
		return false;
	}

	this->m_shaderProgram->useProgram();

	// shader attributes binding
	const GLuint programId = this->m_shaderProgram->programId();

	SceneManager *manager = SceneManager::Instance();
	manager->m_vertexHandle = 0;
	manager->m_normalHandle = 1;
	manager->m_uvHandle = 2;

	// =================================
	manager->m_modelMatHandle = 0;
	manager->m_viewMatHandle = 7;
	manager->m_projMatHandle = 8;
	manager->m_terrainVToUVMatHandle = 9;

	manager->m_albedoMapHandle = 4;
	manager->m_albedoMapTexIdx = 0;
	glUniform1i(manager->m_albedoMapHandle, manager->m_albedoMapTexIdx);

	manager->m_elevationMapHandle = 5;
	manager->m_elevationMapTexIdx = 3;
	glUniform1i(manager->m_elevationMapHandle, manager->m_elevationMapTexIdx);
	
	manager->m_normalMapHandle = 6;
	manager->m_normalMapTexIdx = 2;
	glUniform1i(manager->m_normalMapHandle, manager->m_normalMapTexIdx);
	
	manager->m_albedoTexUnit = GL_TEXTURE0;
	manager->m_elevationTexUnit = GL_TEXTURE3;
	manager->m_normalTexUnit = GL_TEXTURE2;

	manager->m_vs_vertexProcessIdHandle = 1;
	manager->m_vs_commonProcess = 0;
	manager->m_vs_terrainProcess = 3;

	manager->m_fs_pixelProcessIdHandle = 2;
	manager->m_fs_pureColor = 5;
	manager->m_fs_terrainPass = 7;
	
	return true;
}
