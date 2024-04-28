#include "LocalMatrixDraw.h"

void LocalMatrixDraw::Initialize(
	LocalMatrixManager* localMatrixManager, 
	WorldTransform* worldTransform)
{

	localMatrixManager_ = localMatrixManager;

	worldTransform_ = worldTransform;

}

void LocalMatrixDraw::Draw(BaseCamera& camera)
{

	//for (uint32_t i = 0; i < drawLines_.size(); ++i) {
	//	// 親
	//	int32_t parentIndex = localMatrixManager_->nodeDatas_[i].parentIndex;
	//	if (parentIndex == -1) {
	//		// 親ないので帰る
	//		continue;
	//	}
	//	Vector3 startPosition = {
	//	localMatrixManager_->nodeDatas_[parentIndex].matrix.m[3][0],
	//	localMatrixManager_->nodeDatas_[parentIndex].matrix.m[3][1],
	//	localMatrixManager_->nodeDatas_[parentIndex].matrix.m[3][2] };

	//	startPosition = Matrix4x4::Transform(startPosition, worldTransform_->worldMatrix_);

	//	// 本体
	//	Vector3 endPosition = {
	//	localMatrixManager_->nodeDatas_[i].matrix.m[3][0],
	//	localMatrixManager_->nodeDatas_[i].matrix.m[3][1],
	//	localMatrixManager_->nodeDatas_[i].matrix.m[3][2] };

	//	endPosition = Matrix4x4::Transform(endPosition, worldTransform_->worldMatrix_);

	//	drawLines_[i]->Draw(
	//		startPosition,
	//		endPosition,
	//		Vector4{ 1.0f,1.0f,1.0f,1.0f },
	//		Vector4{ 1.0f,1.0f,1.0f,1.0f },
	//		camera);

	//}

}
