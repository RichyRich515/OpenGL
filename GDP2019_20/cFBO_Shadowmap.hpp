#pragma once

#include <string>
#include "GLCommon.h"

// see: http://www.codinglabs.net/tutorial_opengl_deferred_rendering_shadow_mapping.aspx
// https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

class cFBO_Shadowmap
{
public:
	cFBO_Shadowmap() :
		ID(0),
		depthBuffer_ID(0),
		depthTexture_ID(0),
		width(-1), height(-1) {};

	GLuint ID;

	GLuint depthBuffer_ID;
	GLuint depthTexture_ID;
	GLint width;
	GLint height;

	// Inits the FBP
	bool init(int width, int height, std::string& error);
	bool shutdown(void);

	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string& error);

	void clearBuffers(bool bClearColour = true, bool bClearDepth = true);

	void clearDepthBuffer(void);

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};