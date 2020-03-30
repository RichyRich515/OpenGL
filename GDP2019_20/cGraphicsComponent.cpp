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
	instatiateBaseVariables(obj);
}

cGraphicsComponent::~cGraphicsComponent()
{
}

void cGraphicsComponent::init()
{
}

void cGraphicsComponent::preFrame(float dt, float tt)
{
}

void cGraphicsComponent::update(float dt, float tt)
{
	this->dt = dt;
	this->tt = tt;
}

void cGraphicsComponent::render(float dt, float tt)
{
	// TODO: move this to the game object?
	this->pShader = cShaderManager::getCurrentShader();
	// Tie textures
	{
		cBasicTextureManager* pTextureManager = cBasicTextureManager::getTextureManager();
		GLuint texture_ul = pTextureManager->getTextureIDFromName(this->textures[0].fileName);
		if (texture_ul)
		{
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp00"), 1);
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
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp01"), 2);
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
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp02"), 3);
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
			glActiveTexture(GL_TEXTURE0 + 4);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("textSamp03"), 4);
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
			glActiveTexture(GL_TEXTURE0 + 20);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("heightSamp"), 20);
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
			glActiveTexture(GL_TEXTURE0 + 30);
			glBindTexture(GL_TEXTURE_2D, texture_ul);
			glUniform1i(pShader->getUniformLocID("discardSamp"), 30);
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

void cGraphicsComponent::instatiateBaseVariables(const Json::Value& obj)
{
	visible = obj["visible"] ? obj["visible"].asBool() : true;
	lighting = obj["lighting"] ? obj["lighting"].asBool() : false;
	wireFrame = obj["wireFrame"] ? obj["wireFrame"].asBool() : false;
	color = obj["color"] ? Json::toVec4(obj["color"]) : glm::vec4(1.0f);
	specular = obj["specular"] ? Json::toVec4(obj["specular"]) : glm::vec4(1.0f);

	if (obj["texture"])
	{
		if (obj["texture"]["textures"])
		{
			Json::Value obj_textures = obj["texture"]["textures"];

			for (unsigned i = 0; i < obj["texture"]["textures"].size() && i < MAX_TEXTURES; ++i)
			{
				textures[i].fileName = obj_textures[i]["name"] ? obj_textures[i]["name"].asString() : "";
				textures[i].blend = obj_textures[i]["blend"] ? obj_textures[i]["blend"].asFloat() : 1.0f;
				textures[i].tiling = obj_textures[i]["tiling"] ? obj_textures[i]["tiling"].asFloat() : 1.0f;
				textures[i].xOffset = obj_textures[i]["xOffset"] ? obj_textures[i]["xOffset"].asFloat() : 0.0f;
				textures[i].yOffset = obj_textures[i]["yOffset"] ? obj_textures[i]["yOffset"].asFloat() : 0.0f;
			}
		}
		if (obj["texture"]["discardmap"])
		{
			discardmap.fileName = obj["texture"]["discardmap"]["name"] ? obj["texture"]["discardmap"]["name"].asString() : "";
			discardmap.blend = obj["texture"]["discardmap"]["blend"] ? obj["texture"]["discardmap"]["blend"].asFloat() : 1.0f;
			discardmap.tiling = obj["texture"]["discardmap"]["tiling"] ? obj["texture"]["discardmap"]["tiling"].asFloat() : 1.0f;
			discardmap.xOffset = obj["texture"]["discardmap"]["xOffset"] ? obj["texture"]["discardmap"]["xOffset"].asFloat() : 0.0f;
			discardmap.yOffset = obj["texture"]["discardmap"]["yOffset"] ? obj["texture"]["discardmap"]["yOffset"].asFloat() : 0.0f;
		}
	}
	dt = 0;
	tt = 0;
	pShader = nullptr;
}

void cGraphicsComponent::serializeJSONObject(Json::Value& obj)
{
	// TODO: this
}