#include "MakeParticle.h"
#include <cassert>

MakeParticle* MakeParticle::GetInstance()
{
    return nullptr;
}

IParticle* MakeParticle::Run(ParticleDesc* particleDesc, uint32_t paeticleName)
{
	
	IParticle* particle = nullptr;

	switch (paeticleName)
	{
	case kDefaultParticle:
		particle = new IParticle();
		particle->Initialize(particleDesc);
		break;
	case kCountOfParticleName:
	default:
		assert(0);
		break;
	}

	return particle;

}
