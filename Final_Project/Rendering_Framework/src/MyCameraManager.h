#pragma once

#include "camera\MyOrbitControl.h"
#include <vector>

enum class RenderWidgetKeyCode {
	KEY_W, KEY_A, KEY_S, KEY_D, KEY_T, KEY_Z, KEY_X
};
enum class RenderWidgetMouseButton {
	M_LEFT, M_RIGHT, M_MIDDLE
};
struct Plane
{
	// unit vector
	glm::vec3 normal = { 0.f, 1.f, 0.f };
	// distance from origin to the nearest point in the plane
	float     distance = 0.f;
};
struct Frustum
{
	Plane topFace;
	Plane bottomFace;

	Plane rightFace;
	Plane leftFace;

	Plane farFace;
	Plane nearFace;
};

namespace INANOA {

class MyCameraManager
{
public:
	MyCameraManager();
	virtual ~MyCameraManager();

public:
	void init(const int w, const int h);
	void resize(const int w, const int h);

	void mousePress(const RenderWidgetMouseButton button, const int x, const int y) ;
	void mouseRelease(const RenderWidgetMouseButton button, const int x, const int y) ;
	void mouseMove(const int x, const int y) ;
	void mouseScroll(const double xOffset, const double yOffset) ;
	void keyPress(const RenderWidgetKeyCode key) ;
	void keyRelease(const RenderWidgetKeyCode key) ;

	void teleport(const int idx);

public:
	void updateGodCamera();
	void updatePlayerCamera();
	void updateAirplane();
	void adjustAirplaneHeight(const float terrainHeight);
	void adjustPlayerCameraHeight(const float terrainHeight);

private:
	
	void setupViewports(const int w, const int h);

public:
	glm::mat4 playerViewMatrix() const;
	glm::mat4 playerProjectionMatrix() const;
	glm::vec3 playerViewOrig() const;
	glm::vec3 playerCameraLookCenter() const;

	glm::mat4 godViewMatrix() const;
	glm::mat4 godProjectionMatrix() const;

	//shadow mapping
	glm::mat4 L1ViewMatrix() const;
	glm::mat4 L1ProjectionMatrix() const;
	glm::mat4 L2ViewMatrix() const;
	glm::mat4 L2ProjectionMatrix() const;
	glm::mat4 L3ViewMatrix() const;
	glm::mat4 L3ProjectionMatrix() const;

	glm::mat4 airplaneModelMatrix() const;
	glm::vec3 airplanePosition() const;

	glm::ivec4 playerViewport() const;
	glm::ivec4 godViewport() const;

	float playerCameraNear() const;
	float playerCameraFar() const;


	//plane
	Frustum     playerfrustum;
	float aspect;
	float fovY;
	float PLAYER_PROJ_FAR;

	//shadow mapping
	glm::vec3 Lfront = glm::normalize(glm::vec3(-0.4, -0.5, -0.8));//-0.4, -0.5, -0.8
	glm::vec3 Lright = glm::normalize(glm::cross(Lfront, glm::vec3(0, 1, -0.1)));
	glm::vec3 Ltop = glm::normalize(glm::cross(Lright, Lfront));
	glm::mat4 light1_proj_matrix;
	glm::mat4 light1_view_matrix;
	glm::mat4 light2_proj_matrix;
	glm::mat4 light2_view_matrix;
	glm::mat4 light3_proj_matrix;
	glm::mat4 light3_view_matrix;

private:
	glm::mat4 m_airplaneModelMat;

	MyCamera* m_playerMyCamera = nullptr;
	MyCamera* m_godMyCamera = nullptr;
	MyOrbitControl* m_godCameraControl = nullptr;

	glm::mat4 m_godProjMat;
	glm::mat4 m_playerProjMat;

	float m_playerCameraHeightOffset = 0.0;
	float m_playerCameraForwardSpeed = 0.5;

	const float MIN_PLAYER_CAMERA_TERRAIN_OFFSET;
	const float MIN_AIRPLANE_TERRAIN_OFFSET;

	glm::ivec4 m_godViewport;
	glm::ivec4 m_playerViewport;

private:
	bool m_WPressedFlag = false;
	bool m_APressedFlag = false;
	bool m_SPressedFlag = false;
	bool m_DPressedFlag = false;

};


}



