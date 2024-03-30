#include "MyCameraManager.h"
#include <glm\gtc\matrix_transform.hpp>

namespace INANOA {
MyCameraManager::MyCameraManager() : MIN_PLAYER_CAMERA_TERRAIN_OFFSET(5.0), MIN_AIRPLANE_TERRAIN_OFFSET(3.0)
{
}


MyCameraManager::~MyCameraManager()
{
}

void MyCameraManager::init(const int w, const int h){
	// initialize camera and camera control
	this->m_godCameraControl = new INANOA::MyOrbitControl(w, h);
	this->m_godMyCamera = new INANOA::MyCamera(glm::vec3(50.0, 120.0, 100.0), glm::vec3(50.0, 0.0, 60.0), glm::vec3(0.0, 1.0, 0.0), -1.0);
	this->m_godCameraControl->setCamera(this->m_godMyCamera);
	this->m_playerCameraForwardSpeed = 0.4;
	// initialize player camera
	this->m_playerMyCamera = new INANOA::MyCamera(glm::vec3(50.0, 50.0, 120.0), glm::vec3(50.0, 47.0, 110.0), glm::vec3(0.0, 1.0, 0.0), -1.0);

	this->resize(w, h);
}
void MyCameraManager::resize(const int w, const int h){
	// half for god view, half for player view
	PLAYER_PROJ_FAR = 700.0;
	this->setupViewports(w, h);

	this->m_godProjMat = glm::perspective(glm::radians(80.0f), this->m_godViewport[2] * 1.0f / h, 0.1f, 1000.0f);
	this->m_playerProjMat = glm::perspective(glm::radians(45.0f), this->m_playerViewport[2] * 1.0f / h, 0.1f, PLAYER_PROJ_FAR);
	aspect = this->m_playerViewport[2] * 1.0f / h;
	fovY=glm::radians(45.0f);
	// trackball use full screen
	this->m_godCameraControl->resize(w, h);
}

void MyCameraManager::updateGodCamera() {
	this->m_godCameraControl->update();
	this->m_godMyCamera->update();
}


void MyCameraManager::mousePress(const RenderWidgetMouseButton button, const int x, const int y) {
	if (button == RenderWidgetMouseButton::M_LEFT) {
		this->m_godCameraControl->setFunction(INANOA::MyTrackballFunction::ROTATE);
		this->m_godCameraControl->mousePress(x, y);
	}
	else if (button == RenderWidgetMouseButton::M_RIGHT) {
		this->m_godCameraControl->setFunction(INANOA::MyTrackballFunction::PAN);
		this->m_godCameraControl->mousePress(x, y);
	}
}
void MyCameraManager::mouseRelease(const RenderWidgetMouseButton button, const int x, const int y) {
	if (button == RenderWidgetMouseButton::M_LEFT) {
		this->m_godCameraControl->mouseRelease(x, y);
	}
	else if (button == RenderWidgetMouseButton::M_RIGHT) {
		this->m_godCameraControl->mouseRelease(x, y);
	}
}
void MyCameraManager::mouseMove(const int x, const int y) {
	this->m_godCameraControl->mouseMove(x, y);
}
void MyCameraManager::mouseScroll(const double xOffset, const double yOffset) {
	this->m_godCameraControl->mouseScroll(yOffset);
}
void MyCameraManager::keyPress(const RenderWidgetKeyCode key) {
	if (key == RenderWidgetKeyCode::KEY_W) {
		this->m_WPressedFlag = true;
	}
	else if (key == RenderWidgetKeyCode::KEY_S) {
		this->m_SPressedFlag = true;
	}
	else if (key == RenderWidgetKeyCode::KEY_A) {
		this->m_APressedFlag = true;
	}
	else if (key == RenderWidgetKeyCode::KEY_D) {
		this->m_DPressedFlag = true;
	}
	else if (key == RenderWidgetKeyCode::KEY_Z) {
		this->m_playerCameraHeightOffset = 0.1;
	}
	else if (key == RenderWidgetKeyCode::KEY_X) {
		this->m_playerCameraHeightOffset = -0.1;
	}	
}
void MyCameraManager::keyRelease(const RenderWidgetKeyCode key) {
	if (key == RenderWidgetKeyCode::KEY_W) {
		this->m_WPressedFlag = false;
	}
	else if (key == RenderWidgetKeyCode::KEY_S) {
		this->m_SPressedFlag = false;
	}
	else if (key == RenderWidgetKeyCode::KEY_A) {
		this->m_APressedFlag = false;
	}
	else if (key == RenderWidgetKeyCode::KEY_D) {
		this->m_DPressedFlag = false;
	}

	if (key == RenderWidgetKeyCode::KEY_Z || key == RenderWidgetKeyCode::KEY_X) {
		this->m_playerCameraHeightOffset = 0.0;
	}
}

void MyCameraManager::updatePlayerCamera() {

	// update player camera
	if (this->m_WPressedFlag) {
		glm::vec3 before = this->m_playerMyCamera->lookCenter();
		this->m_playerMyCamera->forward(glm::vec3(0.0, 0.0, -1.0) * this->m_playerCameraForwardSpeed, true);
		glm::vec3 after = this->m_playerMyCamera->lookCenter();

		this->m_godMyCamera->translateLookCenterAndViewOrg(after - before);
	}
	else if (this->m_SPressedFlag) {
		glm::vec3 before = this->m_playerMyCamera->lookCenter();
		this->m_playerMyCamera->forward(glm::vec3(0.0, 0.0, 1.0) * this->m_playerCameraForwardSpeed, true);
		glm::vec3 after = this->m_playerMyCamera->lookCenter();

		this->m_godMyCamera->translateLookCenterAndViewOrg(after - before);
	}
	else if (this->m_APressedFlag) {
		this->m_playerMyCamera->rotateLookCenterAccordingToViewOrg(0.01);
	}
	else if (this->m_DPressedFlag) {
		this->m_playerMyCamera->rotateLookCenterAccordingToViewOrg(-0.01);
	}

	this->m_playerMyCamera->translateLookCenterAndViewOrg(glm::vec3(0.0, this->m_playerCameraHeightOffset, 0.0));
	this->m_playerMyCamera->update();	
	glm::vec3 front = glm::normalize(this->m_playerMyCamera->lookCenter() - this->m_playerMyCamera->viewOrig()) * PLAYER_PROJ_FAR;
	glm::vec3 up = this->m_playerMyCamera->upVector();
	glm::vec3 right = glm::normalize(glm::cross(up, front));
	up = glm::normalize(glm::cross(front, right));
	float halfVSide = PLAYER_PROJ_FAR * tanf(fovY * .5f);
	float halfHSide = halfVSide * aspect;
	glm::vec3 frontMultFar = front;
	this->playerfrustum.rightFace.normal = glm::normalize(glm::cross(front - right * halfHSide, up));
	this->playerfrustum.rightFace.distance = glm::dot(this->m_playerMyCamera->viewOrig(), this->playerfrustum.rightFace.normal);
	this->playerfrustum.leftFace.normal = glm::normalize(glm::cross(up, front + right * halfHSide));
	this->playerfrustum.leftFace.distance = glm::dot(this->m_playerMyCamera->viewOrig(), this->playerfrustum.leftFace.normal);
	this->playerfrustum.topFace.normal = glm::normalize(glm::cross(frontMultFar + up * halfVSide, right));
	this->playerfrustum.topFace.distance = glm::dot(this->m_playerMyCamera->viewOrig(), this->playerfrustum.topFace.normal);
	this->playerfrustum.bottomFace.normal = glm::normalize(glm::cross(right,frontMultFar - up * halfVSide));
	this->playerfrustum.bottomFace.distance = glm::dot(this->m_playerMyCamera->viewOrig(), this->playerfrustum.bottomFace.normal);
	this->playerfrustum.nearFace.normal = -glm::normalize(front);
	this->playerfrustum.nearFace.distance = glm::dot(this->m_playerMyCamera->viewOrig(), this->playerfrustum.nearFace.normal)-0.1f;
	this->playerfrustum.farFace.normal = glm::normalize(front);
	this->playerfrustum.farFace.distance = glm::dot(this->m_playerMyCamera->viewOrig(), this->playerfrustum.farFace.normal)+PLAYER_PROJ_FAR;
	//shadow mapping
	glm::vec3 LTP = (front + up * halfVSide + right * halfHSide) / PLAYER_PROJ_FAR;
	glm::vec3 RTP = (front + up * halfVSide - right * halfHSide) / PLAYER_PROJ_FAR;
	glm::vec3 LBP = (front - up * halfVSide + right * halfHSide) / PLAYER_PROJ_FAR;
	glm::vec3 RBP = (front - up * halfVSide - right * halfHSide) / PLAYER_PROJ_FAR;
	std::vector<glm::vec3> AP;
	float z = 0.1;
	AP.push_back(LTP * z);
	AP.push_back(RTP * z);
	AP.push_back(LBP * z);
	AP.push_back(RBP * z);
	z = 50.0;
	AP.push_back(LTP * z);
	AP.push_back(RTP * z);
	AP.push_back(LBP * z);
	AP.push_back(RBP * z);
	float max_Lfront = 500;
	float min_Lfront = glm::dot(Lfront, AP[0]);
	float max_Lright = glm::dot(Lright, AP[0]);
	float min_Lright = glm::dot(Lright, AP[0]);
	float max_Ltop = glm::dot(Ltop, AP[0]);
	float min_Ltop = glm::dot(Ltop, AP[0]);
	for (int i = 1; i < 8; i++) {
		max_Lfront = fmax(max_Lfront, glm::dot(Lfront, AP[i]));
		min_Lfront = fmin(min_Lfront, glm::dot(Lfront, AP[i]));
		max_Lright = fmax(max_Lright, glm::dot(Lright, AP[i]));
		min_Lright = fmin(min_Lright, glm::dot(Lright, AP[i]));
		max_Ltop = fmax(max_Ltop, glm::dot(Ltop, AP[i]));
		min_Ltop = fmin(min_Ltop, glm::dot(Ltop, AP[i]));
	}
	light1_proj_matrix = glm::ortho(min_Lright, max_Lright, min_Ltop, max_Ltop, min_Lfront, max_Lfront);
	light1_view_matrix = glm::lookAt(this->m_playerMyCamera->viewOrig() + glm::vec3(0, 0, 0), this->m_playerMyCamera->viewOrig() + Lfront, Ltop);
	AP.clear();
	z = 50.0;
	AP.push_back(LTP * z);
	AP.push_back(RTP * z);
	AP.push_back(LBP * z);
	AP.push_back(RBP * z);
	z = 200;
	AP.push_back(LTP * z);
	AP.push_back(RTP * z);
	AP.push_back(LBP * z);
	AP.push_back(RBP * z);
	max_Lfront = 500;
	min_Lfront = glm::dot(Lfront, AP[0]);
	max_Lright = glm::dot(Lright, AP[0]);
	min_Lright = glm::dot(Lright, AP[0]);
	max_Ltop = glm::dot(Ltop, AP[0]);
	min_Ltop = glm::dot(Ltop, AP[0]);
	for (int i = 1; i < 8; i++) {
		max_Lfront = fmax(max_Lfront, glm::dot(Lfront, AP[i]));
		min_Lfront = fmin(min_Lfront, glm::dot(Lfront, AP[i]));
		max_Lright = fmax(max_Lright, glm::dot(Lright, AP[i]));
		min_Lright = fmin(min_Lright, glm::dot(Lright, AP[i]));
		max_Ltop = fmax(max_Ltop, glm::dot(Ltop, AP[i]));
		min_Ltop = fmin(min_Ltop, glm::dot(Ltop, AP[i]));
	}
	light2_proj_matrix = glm::ortho(min_Lright, max_Lright, min_Ltop, max_Ltop, min_Lfront, max_Lfront);
	light2_view_matrix = glm::lookAt(this->m_playerMyCamera->viewOrig() + glm::vec3(0, 0, 0), this->m_playerMyCamera->viewOrig() + Lfront, Ltop);
	AP.clear();
	z = 200.0;
	AP.push_back(LTP * z);
	AP.push_back(RTP * z);
	AP.push_back(LBP * z);
	AP.push_back(RBP * z);
	z = 500.0;
	AP.push_back(LTP * z);
	AP.push_back(RTP * z);
	AP.push_back(LBP * z);
	AP.push_back(RBP * z);
	max_Lfront = 500;
	min_Lfront = glm::dot(Lfront, AP[0]);
	max_Lright = glm::dot(Lright, AP[0]);
	min_Lright = glm::dot(Lright, AP[0]);
	max_Ltop = glm::dot(Ltop, AP[0]);
	min_Ltop = glm::dot(Ltop, AP[0]);
	for (int i = 1; i < 8; i++) {
		max_Lfront = fmax(max_Lfront, glm::dot(Lfront, AP[i]));
		min_Lfront = fmin(min_Lfront, glm::dot(Lfront, AP[i]));
		max_Lright = fmax(max_Lright, glm::dot(Lright, AP[i]));
		min_Lright = fmin(min_Lright, glm::dot(Lright, AP[i]));
		max_Ltop = fmax(max_Ltop, glm::dot(Ltop, AP[i]));
		min_Ltop = fmin(min_Ltop, glm::dot(Ltop, AP[i]));
	}
	light3_proj_matrix = glm::ortho(min_Lright, max_Lright, min_Ltop, max_Ltop, min_Lfront, max_Lfront);
	light3_view_matrix = glm::lookAt(this->m_playerMyCamera->viewOrig() + glm::vec3(0, 0, 0), this->m_playerMyCamera->viewOrig() + Lfront, Ltop);

}
void MyCameraManager::updateAirplane() {
	const glm::mat4 playerVM = this->m_playerMyCamera->viewMatrix();
	const glm::vec3 playerViewOrg = this->m_playerMyCamera->viewOrig();

	glm::vec3 vTangent = glm::vec3(-1.0 * playerVM[0].z, -0.2, -1.0 * playerVM[2].z);
	vTangent = glm::normalize(vTangent);
	glm::vec3 airplaneCenter = playerViewOrg + vTangent * 40.0f;	

	// update airplane orientation
	glm::mat4 rm = this->m_playerMyCamera->viewMatrix();
	rm = glm::transpose(rm);
	glm::vec3 bitangent = glm::normalize(glm::cross(glm::vec3(0.0, 1.0, 0.0), vTangent));
	glm::vec3 normal = glm::normalize(glm::cross(vTangent, bitangent));
	glm::mat4 rotMat(1.0);
	rotMat[0] = glm::vec4(bitangent, 0.0);
	rotMat[1] = glm::vec4(normal, 0.0);
	rotMat[2] = glm::vec4(vTangent, 0.0);
	rotMat[3] = glm::vec4(airplaneCenter.x, airplaneCenter.y, airplaneCenter.z, 1.0);

	this->m_airplaneModelMat = rotMat;
}

void MyCameraManager::adjustAirplaneHeight(const float terrainHeight) {
	float airplaneHeight = this->m_airplaneModelMat[3].y;

	if (airplaneHeight - this->MIN_PLAYER_CAMERA_TERRAIN_OFFSET < terrainHeight) {
		airplaneHeight = terrainHeight + MIN_PLAYER_CAMERA_TERRAIN_OFFSET;
	}

	this->m_airplaneModelMat[3].y = airplaneHeight;
}
void MyCameraManager::adjustPlayerCameraHeight(const float terrainHeight) {
	// adjust with terrain height
	const glm::vec3 playerViewOrg_A = this->m_playerMyCamera->viewOrig();
	const float ELEVATION_THRESHOLD = terrainHeight + this->MIN_AIRPLANE_TERRAIN_OFFSET;

	if (playerViewOrg_A.y < ELEVATION_THRESHOLD) {
		this->m_playerMyCamera->translateLookCenterAndViewOrg(glm::vec3(0.0, (ELEVATION_THRESHOLD - playerViewOrg_A.y), 0.0));
	}

	this->m_playerMyCamera->update();
}
void MyCameraManager::setupViewports(const int w, const int h) {
	this->m_playerViewport[2] = w * 0.5;
	this->m_playerViewport[0] = w - this->m_playerViewport[2];
	this->m_playerViewport[1] = 0;
	this->m_playerViewport[3] = h;

	this->m_godViewport[2] = this->m_playerViewport[2];
	this->m_godViewport[0] = 0;
	this->m_godViewport[1] = 0;
	this->m_godViewport[3] = h;
}
void MyCameraManager::teleport(const int idx) {
	auto tele = [](INANOA::MyCamera* godCamera, INANOA::MyCamera* playerCamera, const glm::vec3& viewOrg, const glm::vec3& lookCenter) {
		const glm::vec3 moveVec = lookCenter - playerCamera->lookCenter();
		playerCamera->reset(viewOrg, lookCenter, glm::vec3(0.0, 1.0, 0.0), -1.0);
		godCamera->translateLookCenterAndViewOrg(moveVec);
	};

	if (idx == 0) {
		tele(this->m_godMyCamera, this->m_playerMyCamera, glm::vec3(50.0, 50.0, 120.0), glm::vec3(50.0, 47.0, 110.0));
	}
	else if (idx == 1) {
		tele(this->m_godMyCamera, this->m_playerMyCamera, glm::vec3(30.2523, 49.9996, 498.473), glm::vec3(26.8622, 47.2784, 488.98));
	}
	else if (idx == 2) {
		tele(this->m_godMyCamera, this->m_playerMyCamera, glm::vec3(119.602, 49.9999, 940.079), glm::vec3(115.248, 47.2317, 931.003));
	}
	
}

// ===============================
glm::mat4 MyCameraManager::playerViewMatrix() const { return this->m_playerMyCamera->viewMatrix(); }
glm::mat4 MyCameraManager::playerProjectionMatrix() const { return this->m_playerProjMat; }
glm::vec3 MyCameraManager::playerViewOrig() const { return this->m_playerMyCamera->viewOrig(); }
glm::vec3 MyCameraManager::playerCameraLookCenter() const { return this->m_playerMyCamera->lookCenter(); }

glm::mat4 MyCameraManager::godViewMatrix() const { return this->m_godMyCamera->viewMatrix(); }
glm::mat4 MyCameraManager::godProjectionMatrix() const { return this->m_godProjMat; }

//shadow mapping
glm::mat4 MyCameraManager::L1ViewMatrix() const { return this->light1_view_matrix; }
glm::mat4 MyCameraManager::L1ProjectionMatrix() const { return this->light1_proj_matrix; }
glm::mat4 MyCameraManager::L2ViewMatrix() const { return this->light2_view_matrix; }
glm::mat4 MyCameraManager::L2ProjectionMatrix() const { return this->light2_proj_matrix; }
glm::mat4 MyCameraManager::L3ViewMatrix() const { return this->light3_view_matrix; }
glm::mat4 MyCameraManager::L3ProjectionMatrix() const { return this->light3_proj_matrix; }

glm::mat4 MyCameraManager::airplaneModelMatrix() const { return this->m_airplaneModelMat; }
glm::vec3 MyCameraManager::airplanePosition() const { return glm::vec3(this->m_airplaneModelMat[3]); }

glm::ivec4 MyCameraManager::playerViewport() const { return this->m_playerViewport; }
glm::ivec4 MyCameraManager::godViewport() const { return this->m_godViewport; }

float MyCameraManager::playerCameraNear() const { return 0.1; }
float MyCameraManager::playerCameraFar() const { return 400.0; }

}

