#include "OutLineData.h"

#include "../base/BufferResource.h"
#include "../base/WinApp.h"
#include "../base/DirectXCommon.h"

void OutLineData::Initialize() {
	width_ = 0.05f;
	color_ = {0,0,0,1.0f};
	forVSResource_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(forVS) + 0xff) & ~0xff);
	forVSResource_->Map(0, nullptr, reinterpret_cast<void**>(&forVSData_));
	forVSData_->scaleMatrix = Matrix4x4Calc::MakeScaleMatrix(Vector3{width_,width_ ,width_ });

	forPSResource_ = BufferResource::CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), (sizeof(forPS) + 0xff) & ~0xff);
	forPSResource_->Map(0, nullptr, reinterpret_cast<void**>(&forPSData_));
	forPSData_->color = color_;

}
void OutLineData::TransfarData() {

}
void OutLineData::Map() {
	forVSData_->scaleMatrix = Matrix4x4Calc::MakeScaleMatrix(Vector3{ width_,width_ ,width_ });
	forPSData_->color = color_;
}
