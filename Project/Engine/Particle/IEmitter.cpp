#include "IEmitter.h"
#include "../Math/DeltaTime.h"
#include "../../Application/Particle/MakeParticle.h"

void IEmitter::Initialize(const EmitterDesc& emitterDesc)
{

	// トランスフォーム
	transform_ = *emitterDesc.transform;
	// 発生させるインスタンス数
	instanceCount_ = emitterDesc.instanceCount;
	// 発生頻度
	frequency_ = emitterDesc.frequency;
	// 発生時間
	frequencyTime_ = 0.0f;
	// 生存時間
	lifeTime_ = emitterDesc.lifeTime;
	// 発生してからの経過時間
	currentTime_ = 0.0f;
	// エミットする
	toEmit_ = false;

	isDead_ = false;

	particleModelNum_ = emitterDesc.particleModelNum;
	
	paeticleName_ = emitterDesc.paeticleName;

}

void IEmitter::Update()
{

	frequencyTime_ += kDeltaTime_;
	if (frequencyTime_ >= frequency_) {
		frequencyTime_ = frequencyTime_ - frequency_;
		toEmit_ = true;
	}
	currentTime_ += kDeltaTime_;
	if (currentTime_ >= lifeTime_) {
		isDead_ = true;
	}

}

std::list<IParticle*> IEmitter::Emit()
{

	std::list<IParticle*> particles;
	MakeParticle* makeParticle = MakeParticle::GetInstance();

	for (uint32_t count = 0; count < instanceCount_; ++count) {
		particles.push_back(makeParticle->Run(paeticleName_,transform_.translate, transform_.scale));
	}

	return particles;
}
