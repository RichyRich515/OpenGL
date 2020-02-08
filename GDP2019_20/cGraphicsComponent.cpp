#include "cGraphicsComponent.hpp"
#include "JsonHelper.hpp"
#include "GLCommon.h"
#include "Texture/cBasicTextureManager.h"

cGraphicsComponent::cGraphicsComponent() :
	visible(true),
	lighting(false),
	wireFrame(false),
	color(glm::vec4(1.0f)),
	specular(glm::vec4(1.0f)),
	dt(0.0f),
	tt(0.0f),
	pShader(nullptr)
{
}

cGraphicsComponent::cGraphicsComponent(const Json::Value& obj)
{
	visible = obj["visible"] ? obj["visible"].asBool() : true;
	lighting = obj["lighting"] ? obj["lighting"].asBool() : false;
	wireFrame = obj["wireFrame"] ? obj["wireFrame"].asBool() : false;
	color = obj["color"] ? Json::toVec4(obj["color"]) : glm::vec4(1.0f);
	specular = obj["specular"] ? Json::toVec4(obj["specular"]) : glm::vec4(1.0f);
	dt = 0;
	tt = 0;
	pShader = nullptr;
}

cGraphicsComponent::~cGraphicsComponent()
{
}

void cGraphicsComponent::init()
{
}

void cGraphicsComponent::preFrame()
{
}

void cGraphicsComponent::update(float dt, float tt)
{
	this->dt = dt;
	this->tt = tt;
}

void cGraphicsComponent::render()
{
	// TODO: move this to the game object?
	cShaderManager::setCurrentShader(this->pShader);
	// Tie textures
	{
		cBasicTextureManager* pTextureManager = cBasicTextureManager::getTextureManager();
		GLuint texture_ul = pTextureManager->getTextureIDFromName(this->textures[0].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp00"), 0);
			glUniform4f(pShader->getUniformLocID("textparams00"),
				this->textures[0].xOffset,
				this->textures[0].yOffset,
				this->textures[0].blend,
				this->textures[0].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams00"), 0.0f, 0.0f, 0.0f, 0.0f);
		}
		texture_ul = pTextureManager->getTextureIDFromName(this->textures[1].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp01"), 1);
			glUniform4f(pShader->getUniformLocID("textparams01"),
				this->textures[1].xOffset,
				this->textures[1].yOffset,
				this->textures[1].blend,
				this->textures[1].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams01"), 0.0f, 0.0f, 0.0f, 0.0f);
		}
		texture_ul = pTextureManager->getTextureIDFromName(this->textures[2].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp02"), 2);
			glUniform4f(pShader->getUniformLocID("textparams02"),
				this->textures[2].xOffset,
				this->textures[2].yOffset,
				this->textures[2].blend,
				this->textures[2].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams02"), 0.0f, 0.0f, 0.0f, 0.0f);
		}
		texture_ul = pTextureManager->getTextureIDFromName(this->textures[3].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp03"), 3);
			glUniform4f(pShader->getUniformLocID("textparams03"),
				this->textures[3].xOffset,
				this->textures[3].yOffset,
				this->textures[3].blend,
				this->textures[3].tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("textparams03"), 0.0f, 0.0f, 0.0f, 0.0f);
		}

		// Height map
		texture_ul = pTextureManager->getTextureIDFromName(this->heightmap.fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 40);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("heightSamp"), 40);
			glUniform4f(pShader->getUniformLocID("heightparams"),
				this->heightmap.xOffset,
				this->heightmap.yOffset,
				this->heightmap.blend,
				this->heightmap.tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("heightparams"), 0.0f, 0.0f, 0.0f, 0.0f);
		}

		// discard map
		texture_ul = pTextureManager->getTextureIDFromName(this->discardmap.fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 50);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("discardSamp"), 50);
			glUniform4f(pShader->getUniformLocID("discardparams"),
				this->discardmap.xOffset,
				this->discardmap.yOffset,
				this->discardmap.blend,
				this->discardmap.tiling);
		}
		else
		{
			glUniform4f(pShader->getUniformLocID("discardparams"), 0.0f, 0.0f, 0.0f, 0.0f);
		}

	}

	glUniform4f(pShader->getUniformLocID("diffuseColour"), this->color.r, this->color.g, this->color.b, this->color.a);
	glUniform4f(pShader->getUniformLocID("specularColour"), this->specular.r, this->specular.g, this->specular.b, this->specular.a);
	glUniform4f(pShader->getUniformLocID("params1"), dt, tt, (float)this->lighting, 0.0f);

	if (this->wireFrame)
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		// glCullFace
		//glEnable(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

eComponentType cGraphicsComponent::getType()
{
	return eComponentType::Graphics;
}

void cGraphicsComponent::instatiateBaseVariables(Json::Value& obj)
{

}

void cGraphicsComponent::serializeJSONObject(Json::Value& obj)
{
	// TODO: this
}