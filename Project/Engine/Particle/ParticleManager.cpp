#include "ParticleManager.h"
#include <d3d12.h>
#include "../base/TextureManager.h"
#include "../3D/Model.h"
#include "../Math/DeltaTime.h"
#include "../../Application/Particle/MakeEmitter.h"
#include "../base/SRVDescriptorHerpManager.h"
#include "../3D/ModelDraw.h"

#include <cassert>

uint32_t ParticleManager::kNumInstanceMax_ = 1024;

ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
	
}

void ParticleManager::Initialize(ID3D12RootSignature* rootSignature,
	ID3D12PipelineState* pipelineState)
{

	rootSignature_ = rootSignature;
	pipelineState_ = pipelineState;

	//WVP用のリソースを作る。
	particleForGPUBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), ((sizeof(ParticleForGPU) + 0xff) & ~0xff) * kNumInstanceMax_);
	//書き込むためのアドレスを取得
	particleForGPUBuff_->Map(0, nullptr, reinterpret_cast<void**>(&particleForGPUMap_));

	for (size_t i = 0; i < kNumInstanceMax_; i++) {
		particleForGPUMap_[i].World = Matrix4x4::MakeIdentity4x4();
		particleForGPUMap_[i].WVP = Matrix4x4::MakeIdentity4x4();
		particleForGPUMap_[i].color = {1.0f,1.0f,1.0f,1.0f};
	}

	SRVCreate();

	billBoardMatrix_ = Matrix4x4::MakeIdentity4x4();
	billBoardMatrixX_ = Matrix4x4::MakeIdentity4x4();
	billBoardMatrixY_ = Matrix4x4::MakeIdentity4x4();
	billBoardMatrixZ_ = Matrix4x4::MakeIdentity4x4();

	for (size_t i = 0; i < particleDatas_.size(); i++) {
		particleDatas_[i].instanceIndex_ = 0;
		//スタートインスタンス用のリソースを作る。
		particleDatas_[i].startInstanceIdBuff_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(StartInstanceId) + 0xff) & ~ 0xff);
		//書き込むためのアドレスを取得
		particleDatas_[i].startInstanceIdBuff_->Map(0, nullptr, reinterpret_cast<void**>(&particleDatas_[i].startInstanceIdMap_));
		particleDatas_[i].startInstanceIdMap_->num = 0;
	}

}

void ParticleManager::SRVCreate()
{

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumInstanceMax_;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instancingSrvHandleCPU_ = SRVDescriptorHerpManager::GetCPUDescriptorHandle();
	instancingSrvHandleGPU_ = SRVDescriptorHerpManager::GetGPUDescriptorHandle();
	SRVDescriptorHerpManager::NextIndexDescriptorHeapChange();
	DirectXCommon::GetInstance()->GetDevice()->CreateShaderResourceView(particleForGPUBuff_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);

}

void ParticleManager::Update(BaseCamera& camera)
{

	DeadDelete();

	BillBoardUpdate(camera);

	EmitterUpdate();

	ParticlesUpdate();

}

void ParticleManager::Draw(const Matrix4x4& viewProjectionMatrix, ID3D12GraphicsCommandList* commandList)
{

	assert(commandList);

	Map(viewProjectionMatrix);

	//RootSignatureを設定。
	commandList->SetPipelineState(pipelineState_);//PS0を設定
	commandList->SetGraphicsRootSignature(rootSignature_);

	// SRV
	ID3D12DescriptorHeap* ppHeaps[] = { SRVDescriptorHerpManager::descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	//形状を設定。PS0に設定しているものとは別。
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (uint32_t i = 0; i < kCountofParticleModelIndex; i++) {

		commandList->IASetVertexBuffers(0, 1, particleDatas_[i].model_->GetMesh()->GetVbView()); //VBVを設定

		//マテリアルを設定
		commandList->SetGraphicsRootConstantBufferView(0, Model::GetDefaultMaterial()->GetMaterialBuff()->GetGPUVirtualAddress());

		// 開始位置を設定
		commandList->SetGraphicsRootConstantBufferView(3, particleDatas_[i].startInstanceIdBuff_->GetGPUVirtualAddress());

		//SRVのDescriptorTableの先頭を設定。2はrootParamenter[2]である
		for (size_t j = 0; j < particleDatas_[i].model_->GetModelData().material.textureFilePaths.size(); ++j) {
			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, 2, particleDatas_[i].model_->GetTextureHandles()[j]);
		}

		commandList->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

		//描画
		commandList->DrawInstanced(UINT(particleDatas_[i].model_->GetModelData().vertices.size()), particleDatas_[i].instanceIndex_, 0, 0);

	}

}

void ParticleManager::Map(const Matrix4x4& viewProjectionMatrix)
{

	uint32_t instanceIndex = 0u;

	for (uint32_t i = 0; i < kCountofParticleModelIndex; i++) {
		particleDatas_[i].startInstanceIdMap_->num = instanceIndex;
		std::list<IParticle*>::iterator itr = particleDatas_[i].particles_.begin();
		for (; itr != particleDatas_[i].particles_.end(); ++itr) {
			IParticle* particle = *itr;
			particleForGPUMap_[instanceIndex] = particle->Map(viewProjectionMatrix, particleDatas_[i].model_->GetRootNodeLocalMatrix());
			instanceIndex++;
		}
	}

}

void ParticleManager::Finalize()
{
	for (uint32_t i = 0; i < kCountofParticleModelIndex; i++) {
		particleDatas_[i].particles_.remove_if([](IParticle* particle) {
			delete particle;
			return true;
			});
		particleDatas_[i].startInstanceIdMap_->num = 0;
	}
	emitters_.remove_if([](IEmitter* emitter) {
		delete emitter;
		return true;
	});

	for (size_t i = 0; i < kNumInstanceMax_; i++) {
		particleForGPUMap_[i].World = Matrix4x4::MakeIdentity4x4();
		particleForGPUMap_[i].WVP = Matrix4x4::MakeIdentity4x4();
		particleForGPUMap_[i].color = { 1.0f,1.0f,1.0f,1.0f };
	}

}

void ParticleManager::ModelCreate(std::array<Model*, kCountofParticleModelIndex> model)
{

	for (uint32_t i = 0; i < kCountofParticleModelIndex; i++) {
		particleDatas_[i].model_ = model[i];
	}

}

void ParticleManager::BillBoardUpdate(BaseCamera& camera)
{

	// 全軸
	Matrix4x4 backToFrontMatrix = Matrix4x4::MakeRotateXYZMatrix({ 0.0f, 3.14f, 0.0f });
	billBoardMatrix_ = Matrix4x4::Multiply(backToFrontMatrix, camera.GetTransformMatrix());
	billBoardMatrix_.m[3][0] = 0.0f;
	billBoardMatrix_.m[3][1] = 0.0f;
	billBoardMatrix_.m[3][2] = 0.0f;

	// X
	Matrix4x4 cameraTransformMatrix = Matrix4x4::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f },
		Vector3{ camera.GetRotate().x, 0.0f, 0.0f },
		camera.GetTranslate());
	billBoardMatrixX_ = Matrix4x4::Multiply(backToFrontMatrix, cameraTransformMatrix);
	billBoardMatrixX_.m[3][0] = 0.0f;
	billBoardMatrixX_.m[3][1] = 0.0f;
	billBoardMatrixX_.m[3][2] = 0.0f;

	// Y
	cameraTransformMatrix = Matrix4x4::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f },
		Vector3{ 0.0f, camera.GetRotate().y, 0.0f},
		camera.GetTranslate());
	billBoardMatrixY_ = Matrix4x4::Multiply(backToFrontMatrix, cameraTransformMatrix);
	billBoardMatrixY_.m[3][0] = 0.0f;
	billBoardMatrixY_.m[3][1] = 0.0f;
	billBoardMatrixY_.m[3][2] = 0.0f;

	// Z
	cameraTransformMatrix = Matrix4x4::MakeAffineMatrix(
		{ 1.0f, 1.0f, 1.0f },
		Vector3{ 0.0f, 0.0f, camera.GetRotate().z },
		camera.GetTranslate());
	billBoardMatrixZ_ = Matrix4x4::Multiply(backToFrontMatrix, cameraTransformMatrix);
	billBoardMatrixZ_.m[3][0] = 0.0f;
	billBoardMatrixZ_.m[3][1] = 0.0f;
	billBoardMatrixZ_.m[3][2] = 0.0f;

}

void ParticleManager::MakeEmitter(const EulerTransform& transform, uint32_t instanceCount,
	float frequency, float lifeTime,
	uint32_t particleModelNum, uint32_t paeticleName, uint32_t emitterName)
{
	emitters_.push_back(MakeEmitter::GetInstance()->Run(transform, instanceCount,
		frequency, lifeTime,
		particleModelNum, paeticleName, emitterName));

}

void ParticleManager::EmitterUpdate()
{

	for (IEmitter* emitter : emitters_) {
		emitter->Update();
		if (emitter->GetToEmit()) {
			AddParticles(emitter->Emit(),emitter->GetParticleModelNum());
			emitter->SetToEmit(false);
		}
	}

}

void ParticleManager::AddParticles(std::list<IParticle*> particles, uint32_t particleModelNum)
{

	particleDatas_[particleModelNum].particles_.splice(particleDatas_[particleModelNum].particles_.end(), particles);
	particleDatas_[particleModelNum].instanceIndex_ = static_cast<uint32_t>(particleDatas_[particleModelNum].particles_.size());
}

void ParticleManager::ParticlesUpdate()
{

	for (uint32_t i = 0; i < kCountofParticleModelIndex; i++) {
		for (IParticle* particle : particleDatas_[i].particles_) {
			switch (particle->GetBillBoardName())
			{
			case IParticle::kBillBoardNameIndexAllAxis:
				particle->Update(billBoardMatrix_);
				break;
			case IParticle::kBillBoardNameIndexXAxis:
				particle->Update(billBoardMatrixX_);
				break;
			case IParticle::kBillBoardNameIndexYAxis:
				particle->Update(billBoardMatrixY_);
				break;
			case IParticle::kBillBoardNameIndexZAxis:
				particle->Update(billBoardMatrixZ_);
				break;
			default:
				break;
			}
		}
	}
}

void ParticleManager::DeadDelete()
{
	
	for (uint32_t i = 0; i < kCountofParticleModelIndex; i++) {
		particleDatas_[i].particles_.remove_if([=](IParticle* particle) {
			if (particle->IsDead()) {
				delete particle;
				particleDatas_[i].instanceIndex_--;
				return true;
			}
			return false;
			});
	}

	emitters_.remove_if([](IEmitter* emitter) {
		if (emitter->IsDead()) {
			delete emitter;
			return true;
		}
		return false;
	});

}
