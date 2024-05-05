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
    /// <param name="emitterDesc">エミッタ引数</param>
    /// <param name="emitterName">エミッタの名前</param>
    /// <returns></returns>
    IEmitter* Run(const EmitterDesc& emitterDesc, uint32_t emitterName);

private:
    MakeEmitter() = default;
    ~MakeEmitter() = default;
    MakeEmitter(const MakeEmitter&) = delete;
    MakeEmitter& operator=(const MakeEmitter&) = delete;

};

