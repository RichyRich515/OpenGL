#pragma once

enum class eComponentType
{
	None = 0,
	Graphics = 1,
	Physics
};

class iComponent
{
public:
	virtual ~iComponent() = 0;
	virtual void init() = 0;
	virtual void update(float dt, float tt) = 0;

	virtual eComponentType getType() = 0;
	
};