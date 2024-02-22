#pragma once

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"
#include "../Math/Matrix4x4.h"
#include "../3D/VertexData.h"
#include "../3D/TransformationMatrix.h"
#include "../3D/TransformStructure.h"
#include "../3D/MaterialData.h"
#include "../base/SafeDelete.h"
#include "../base/GraphicsPipelineState/GraphicsPipelineState.h"
//クラス化
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"
#include "../base/TextureManager.h"
#include "../2D/Sprite.h"
#include "../3D/Model.h"
#include "../3D/Material.h"
#include "../Light/DirectionalLight/DirectionalLight.h"
#include "../Light/PointLight/PointLightManager.h"
#include "../Light/SpotLight/SpotLightManager.h"
#include "../base/D3DResourceLeakChecker.h"

// サウンド再生
#include "../Audio/Audio.h"

//入力デバイス
#include "../Input/Input.h"

//デバッグカメラ
#include "../Camera/DebugCamera.h"

// シーンマネージャー
#include "../Scene/SceneManager/SceneManager.h"

// ImGui
#include "../2D/ImGuiManager.h"

// グローバル変数
#include "../GlobalVariables/GlobalVariables.h"

// パーティクル
#include "../Particle/ParticleManager.h"

// アニメーション
#include "../Animation/AnimationFile.h"

// ディスクリプタヒープ
#include ".../../../base/DescriptorHerpManager.h"

class MyFramework
{

public: // メンバ関数

	virtual ~MyFramework() = default;

	// 初期化
	virtual void Initialize();

	// 終了
	virtual void Finalize();

	// 毎フレーム更新
	virtual void Update();

	// 描画
	virtual void Draw() = 0;

	// 終了チェック
	virtual bool IsEndRequst() { return endRequst_; }

public: 

	/// <summary>
	/// ラン
	/// </summary>
	void Run();

protected: // メンバ変数


	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	DescriptorHerpManager* descriptorHerpManager = nullptr;

	Audio* audio = nullptr;
	Input* input = nullptr;

	ImGuiManager* imGuiManager = nullptr;

	bool endRequst_;

};

