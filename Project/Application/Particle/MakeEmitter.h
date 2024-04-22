#pragma once
#include "../../Engine/Particle/IEmitter.h"
#include "../../Engine/Math/Vector3.h"

class MakeEmitter
{

public:
    /// <summary>
    /// シングルトンインスタンスの取得
    /// </summary>
    /// <returns></returns>
    static MakeEmitter* GetInstance();

    /// <summary>
    /// パーティクル作成
    /// </summary>
    /// <returns></returns>
    IEmitter* Run(const EulerTransform& transform, uint32_t instanceCount,
        float frequency, float lifeTime,
        uint32_t particleModelNum, uint32_t paeticleName, uint32_t emitterName);

private:
    MakeEmitter() = default;
    ~MakeEmitter() = default;
    MakeEmitter(const MakeEmitter&) = delete;
    MakeEmitter& operator=(const MakeEmitter&) = delete;

};

