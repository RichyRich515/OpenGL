#pragma once

#include <string>
#include "GLCommon.h"

class cFBO_deferred
{
public:
	cFBO_deferred() :
		ID(0),
		colourBuffer_0_ID(0),
		worldNormalBuffer_1_ID(0),
		worldVertexPositionBuffer_2_ID(0),
		specularBuffer_3_ID(0),
		depthTexture_ID(0),
		width(-1), height(-1) {};

	GLuint ID;						// = 0;

	GLuint colourBuffer_0_ID;		// = 0;
	GLuint worldNormalBuffer_1_ID;
	GLuint worldVertexPositionBuffer_2_ID;
	GLuint specularBuffer_3_ID;

	GLuint depthTexture_ID;			// = 0;
	GLint width;					// = 512 the WIDTH of the framebuffer, in pixels;
	GLint height;

	// Inits the FBP
	bool init(int width, int height, std::string& error);
	bool shutdown(void);
	// Calls shutdown(), then init()
	bool reset(int width, int height, std::string& error);

	void clearBuffers(bool bClearColour = true, bool bClearDepth = true);

	void clearColourBuffer(int bufferindex);
	void clearAllColourBuffers(void);
	void clearDepthBuffer(void);
	void clearStencilBuffer(int clearColour, int mask = 0xFF);

	int getMaxColourAttachments(void);
	int getMaxDrawBuffers(void);
};