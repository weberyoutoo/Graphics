#include "MyImGuiPanel.h"



MyImGuiPanel::MyImGuiPanel()
{
	this->m_avgFPS = 0.0;
	this->m_avgFrameTime = 0.0;
	this->idx = 0;
	this->normalMap_flag = true;
	this->displayMode = 0;
}


MyImGuiPanel::~MyImGuiPanel()
{
}

void MyImGuiPanel::update() {
	// performance information
	const std::string FPS_STR = "FPS: " + std::to_string(this->m_avgFPS);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FPS_STR.c_str());
	const std::string FT_STR = "Frame: " + std::to_string(this->m_avgFrameTime);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FT_STR.c_str());
	// Teleportation button
	if (ImGui::Button("Teleportation"))
	{
		this->idx = (this->idx + 1) % 3;
		if (m_myCameraManager != nullptr)
			m_myCameraManager->teleport(idx);
	}
	const std::string TELE_STR = "Teleport: " + std::to_string(this->idx);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), TELE_STR.c_str());
	// Normal mapping disable/enable button
	/*if (ImGui::Button("Normal map"))
	{
		normalMap_flag = !normalMap_flag;
		myDirectObject->setNormalMapFlag(normalMap_flag);
	}
	const std::string NM_STR[2] = { "Normal mappint: Enable", "Normal mappint: Disable" };
	ImGui::TextColored(ImVec4(0, 220, 0, 255), NM_STR[normalMap_flag].c_str());
	*/
	const std::string NM_STR[2] = { "Disable", "Enable" };
	ImGui::Checkbox("Normal map", &normalMap_flag);
	myDirectObject->setNormalMapFlag(!normalMap_flag);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), NM_STR[normalMap_flag].c_str());

	if (ImGui::Button("Display Mode"))
	{
		displayMode = (displayMode + 1) % 7;
		myGBuffer->setDisplayMode(displayMode);
		if (displayMode == 6)
			myrenderer->mp = 0.5;
		else
			myrenderer->mp = 0.0;
	}
	const std::string DM_STR[7] = { "Blinn-Phone", "World vertex", "World normal", "Diffuse", "Specular", "Ambient" ,"Cascade shadow" };
	ImGui::TextColored(ImVec4(0, 220, 0, 255), DM_STR[displayMode].c_str());

}

void MyImGuiPanel::setAvgFPS(const double avgFPS){
	this->m_avgFPS = avgFPS;
}
void MyImGuiPanel::setAvgFrameTime(const double avgFrameTime){
	this->m_avgFrameTime = avgFrameTime;
}
void MyImGuiPanel::appendCameraManager(INANOA::MyCameraManager *cameraManager) {
	this->m_myCameraManager = cameraManager;
}
void MyImGuiPanel::appendDirectObject(DirectObject *directObj) {
	this->myDirectObject = directObj;
}
void MyImGuiPanel::appendGBufferObject(G_BufferObject *gbuffer) {
	this->myGBuffer = gbuffer;
}
void MyImGuiPanel::appendSceneRenderer(SceneRenderer * renderer) {
	this->myrenderer = renderer;
}