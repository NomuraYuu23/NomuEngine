#include "MakeParticle.h"
#include <cassert>

MakeParticle* MakeParticle::GetInstance()
{
    return nullptr;
}

IParticle* MakeParticle::Run(uint32_t paeticleName, const Vector3& position, const Vector3& size)
{
	
	IParticle* particle = nullptr;

	switch (paeticleName)
	{
	case kDefaultParticle:
		particle = new IParticle();
		particle->Initialize(position, size);
		break;
	case kCountOfParticleName:
	default:
		assert(0);
		break;
	}

	return particle;

}
