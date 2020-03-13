#pragma once
#include "eBodyType.h"

namespace phys
{
	class cCollisionBody
	{
	public:
		virtual ~cCollisionBody() {}

		inline const eBodyType& GetBodyType() { return mBodyType; }

		virtual void ClearAccelerations() = 0;

		// TODO: user pointer

	protected:
		cCollisionBody(eBodyType bodyType) :
			mBodyType(bodyType)
		{}

	private:
		eBodyType mBodyType;

		cCollisionBody() = delete;
		cCollisionBody(const cCollisionBody& other) = delete;
		cCollisionBody& operator=(const cCollisionBody& other) = delete;
	};
}