#pragma once

#include "../../Engine/Particle/IParticle.h"
#include "../../Engine/Math/Vector3.h"

class MakeParticle
{

public:
    /// <summary>
    /// シングルトンインスタンスの取得
    /// </summary>
    /// <returns></returns>
    static MakeParticle* GetInstance();

    /// <summary>
    /// パーティクル作成
    /// </summary>
    /// <returns></returns>
    IParticle* Run(ParticleDesc* particleDesc, uint32_t paeticleName);

private:
    MakeParticle() = default;
    ~MakeParticle() = default;
    MakeParticle(const MakeParticle&) = delete;
    MakeParticle& operator=(const MakeParticle&) = delete;

};

