#pragma once

enum class eComponentType
{
	None = 0,
	GameObject,
	Graphics,
	Physics,
	Transform,
	Mesh
};

class iComponent
{
public:
	virtual ~iComponent() {};
	virtual void init() = 0;

	virtual eComponentType getType() = 0;
	
	virtual void preFrame() = 0;
	virtual void update(float dt, float tt) = 0;
	virtual void render() = 0;
};