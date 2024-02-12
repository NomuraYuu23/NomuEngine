#pragma once
#include "../../Engine/Audio/AudioManager.h"

enum GameAudioNameIndex {
	kGameAudioNameIndexSample, // サンプル
	kGameAudioNameIndexOfCount // 数
};

class GameAudioManager : public AudioManager
{

public: // メンバ関数

	~GameAudioManager();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 音声再生
	/// </summary>
	/// <param name="audioIndex">オーディオの名前</param>
	/// <returns>再生中のサウンドデータの番号</returns>
	void PlayWave(uint32_t audioIndex) override;

private: // メンバ変数

	// データ
	std::array<AudioData, GameAudioNameIndex::kGameAudioNameIndexOfCount> audioDatas_;

	// ファイル名
	std::array<const std::string, GameAudioNameIndex::kGameAudioNameIndexOfCount> audioNames_ = 
	{
		"default/Alarm01.wav",
	};

};

