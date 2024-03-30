#pragma once

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <string>

#include "DirectObject.h"
#include "G_BufferSetting.h"
#include "SceneRenderer.h"

class MyImGuiPanel
{
public:
	MyImGuiPanel();
	virtual ~MyImGuiPanel();

public:
	void update();
	void setAvgFPS(const double avgFPS);
	void setAvgFrameTime(const double avgFrameTime);
	void appendCameraManager(INANOA::MyCameraManager *cameraManager);
	void appendDirectObject(DirectObject *directObj);
	void appendGBufferObject(G_BufferObject *gbuffer);
	void appendSceneRenderer(SceneRenderer* renderer);

private:
	double m_avgFPS;
	double m_avgFrameTime;
	bool normalMap_flag;
	int idx;
	int displayMode;
	INANOA::MyCameraManager *m_myCameraManager = nullptr;
	DirectObject *myDirectObject = nullptr;
	G_BufferObject *myGBuffer = nullptr;
	SceneRenderer * myrenderer = nullptr;
};

