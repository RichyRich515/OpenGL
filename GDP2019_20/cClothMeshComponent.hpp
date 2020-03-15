#include "cMeshComponent.hpp"
#include "iClothComponent.h"

class cClothMeshComponent : public cMeshComponent
{
public:
	cClothMeshComponent();
	~cClothMeshComponent();

	virtual void init() override;
	virtual void preFrame(float dt, float tt) override;
	virtual void update(float dt, float tt) override;
	virtual void render(float dt, float tt) override;
	virtual eComponentType getType() override;

	virtual void instatiateBaseVariables(const Json::Value& obj) override;

	virtual void serializeJSONObject(Json::Value& obj) override;

	nPhysics::iClothComponent* cloth;
};