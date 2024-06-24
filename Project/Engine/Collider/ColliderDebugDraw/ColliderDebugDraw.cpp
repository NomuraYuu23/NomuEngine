#include "ColliderDebugDraw.h"
#include "../../2D/ImguiManager.h"

void ColliderDebugDraw::Initialize()
{

	ListClear();

	isDraw_ = false;

	InitializeOBB();

}

void ColliderDebugDraw::ListClear()
{

	colliders_.clear();

}


void ColliderDebugDraw::AddCollider(const ColliderShape& collider)
{

	colliders_.push_back(collider);

}

void ColliderDebugDraw::DrawMap(DrawLine* drawLine)
{

	if (!isDraw_) {
		return;
	}

	for (std::list<ColliderShape>::iterator itr = colliders_.begin();
		itr != colliders_.end(); ++itr) {

		ColliderShape collider = *itr;

		// OBBなら
		if (std::holds_alternative<OBB>(collider)) {

			OBB obb = std::get<OBB>(collider);
			DrawMapOBB(drawLine, obb);

		}

	}

}

void ColliderDebugDraw::ImGuiDraw()
{
#ifdef _DEBUG
	ImGui::Begin("ColliderDebugDraw");
	ImGui::Checkbox("描画するか", &isDraw_);
	ImGui::End();
#endif // _DEBUG
}

void ColliderDebugDraw::InitializeOBB()
{

	obbOffsetPoints_ = {
		{
			{-1.0f, -1.0f, -1.0f},
			{+1.0f, -1.0f, -1.0f},
			{-1.0f, +1.0f, -1.0f},
			{+1.0f, +1.0f, -1.0f},
			{-1.0f, -1.0f, +1.0f},
			{+1.0f, -1.0f, +1.0f},
			{-1.0f, +1.0f, +1.0f},
			{+1.0f, +1.0f, +1.0f}
		}
	};

}

void ColliderDebugDraw::DrawMapOBB(DrawLine* drawLine, const OBB& collider)
{
	
	std::array<Vector3, 8> points = {};
	
	Vector3 size = collider.size_;
	Matrix4x4 rotateMatrix = Matrix4x4::MakeIdentity4x4();

	rotateMatrix.m[0][0] = collider.otientatuons_[0].x;
	rotateMatrix.m[0][1] = collider.otientatuons_[0].y;
	rotateMatrix.m[0][2] = collider.otientatuons_[0].z;

	rotateMatrix.m[1][0] = collider.otientatuons_[1].x;
	rotateMatrix.m[1][1] = collider.otientatuons_[1].y;
	rotateMatrix.m[1][2] = collider.otientatuons_[1].z;

	rotateMatrix.m[2][0] = collider.otientatuons_[2].x;
	rotateMatrix.m[2][1] = collider.otientatuons_[2].y;
	rotateMatrix.m[2][2] = collider.otientatuons_[2].z;

	Vector3 center = collider.center_;

	for (uint32_t i = 0; i < obbOffsetPoints_.size(); ++i) {
		
		// offset * size
		points[i].x = obbOffsetPoints_[i].x * size.x;
		points[i].y = obbOffsetPoints_[i].y * size.y;
		points[i].z = obbOffsetPoints_[i].z * size.z;

		// 軸で回転
		points[i] = Matrix4x4::TransformNormal(points[i], rotateMatrix);

		// centerに移動
		points[i] += center;

	}

	LineForGPU lineForGPU = {};
	lineForGPU.color[0] = { 1.0f,1.0f,1.0f,1.0f };
	lineForGPU.color[1] = { 1.0f,1.0f,1.0f,1.0f };

	lineForGPU.position[0] = points[0];
	lineForGPU.position[1] = points[1];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[2];
	lineForGPU.position[1] = points[3];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[0];
	lineForGPU.position[1] = points[2];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[1];
	lineForGPU.position[1] = points[3];
	drawLine->Map(lineForGPU);

	lineForGPU.position[0] = points[4];
	lineForGPU.position[1] = points[5];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[6];
	lineForGPU.position[1] = points[7];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[4];
	lineForGPU.position[1] = points[6];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[5];
	lineForGPU.position[1] = points[7];
	drawLine->Map(lineForGPU);

	lineForGPU.position[0] = points[0];
	lineForGPU.position[1] = points[4];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[1];
	lineForGPU.position[1] = points[5];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[2];
	lineForGPU.position[1] = points[6];
	drawLine->Map(lineForGPU);
	lineForGPU.position[0] = points[3];
	lineForGPU.position[1] = points[7];
	drawLine->Map(lineForGPU);

}
