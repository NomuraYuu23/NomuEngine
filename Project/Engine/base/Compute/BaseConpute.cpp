#include "BaseConpute.h"
#include <cassert>
#include "../Log.h"
#include "../CompileShader.h"

ID3D12Device* BaseConpute::device_ = nullptr;

void BaseConpute::Initialize(
	const std::wstring& filePath,
	const wchar_t* entryPoint)
{

	assert(device_);

	// シェーダ
	CreateShader(filePath, entryPoint);
	// ルートシグネチャ
	CreateRootSignature();
	// パイプライン
	CreatePipelineState();

}

void BaseConpute::CommandExecution(DirectXCommon* dxCommon)
{

	//コマンドリストをクローズ、キック
	HRESULT hr = dxCommon->GetCommadList()->Close();
	assert(SUCCEEDED(hr));

	ID3D12CommandList* commandLists[] = { dxCommon->GetCommadList() };
	DxCommand::GetCommandQueue()->ExecuteCommandLists(1, commandLists);

	//実行待ち
	//Fenceの値を更新
	dxCommon->SetFenceVal(dxCommon->GetFenceVal() + 1);
	//GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにSignalを送る
	DxCommand::GetCommandQueue()->Signal(dxCommon->GetFence(), dxCommon->GetFenceVal());

	//Fenceの値が指定したSignal値にたどり着いているが確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (dxCommon->GetFence()->GetCompletedValue() < dxCommon->GetFenceVal()) {
		//FrenceのSignalを持つためのイベントを作成する
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);
		//指定したSignalにたどりついていないので、たどりつくまで待つようにイベントを設定する
		dxCommon->GetFence()->SetEventOnCompletion(dxCommon->GetFenceVal(), fenceEvent);
		//イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	
	//実行が完了したので、アロケータとコマンドリストをリセット
	hr = dxCommon->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(hr));
	hr = dxCommon->GetCommadList()->Reset(dxCommon->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(hr));

}
