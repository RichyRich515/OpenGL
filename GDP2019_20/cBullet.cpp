#include "cBullet.hpp"
#include "cWorld.hpp"
#include <iostream>

void cBullet::update(float dt, float tt)
{
	glm::vec3 pos = this->getPosition();

	if (pos.x > 128.0f || pos.x < -128.0f || pos.z > 128.0f || pos.z < -128.0f)
	{
		cWorld::getWorld()->deferredDeleteGameObject(this);
	}
	else if (from_enemy && glm::distance(pos, cWorld::pCamera->position) <= 0.75f)
	{
		// Enemy bullet
		cWorld::getWorld()->message(sMessage("Reset"));
		std::cout << "dead" << std::endl;
	}
	else if (!from_enemy)
	{
		// player bullet
		std::vector<cGameObject*> demons;
		cWorld::getWorld()->message(sMessage("GetObjects:demon", (void*)&demons));

		for (auto d : demons)
		{
			if (glm::distance(d->getPosition(), pos) < 1.0f)
			{
				cWorld::getWorld()->deferredDeleteGameObject(this);
				cWorld::getWorld()->deferredDeleteGameObject(d);
				break;
			}
		}
	}
}
