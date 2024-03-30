#pragma once

#include "Shader.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

struct DEFERRED
{
	GLint position_map;
	GLint normal_map;
	GLint diffuse_map;
	GLint specular_map;
	GLint ambient_map;

	GLuint programID;
};

const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT4,
								GL_COLOR_ATTACHMENT1,
								GL_COLOR_ATTACHMENT2,
								GL_COLOR_ATTACHMENT3,
								GL_COLOR_ATTACHMENT0 };

class G_BufferObject {

private:
	GLuint rboDepth;
	GLuint fbo;
	GLuint position_map;
	GLuint normal_map;
	GLuint diffuse_map;
	GLuint specular_map;
	GLuint ambient_map;
	GLuint vao;
	GLuint displayMode;
	int mode;

public:
	G_BufferObject();
	virtual ~G_BufferObject();
	bool init(ShaderProgram *program, int FRAME_WIDTH, int FRAME_HEIGHT);
	void resize(int w, int h);
	void prepare();
	void paint();
	void setViewPort(int x, int y, int w, int h);
	void setDisplayMode(int m) { mode = m; };
};