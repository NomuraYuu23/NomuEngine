#include "PostEffect.hlsli"

// 定数データ
struct ComputeParameters {

	uint32_t threadIdOffsetX; // スレッドのオフセットX
	uint32_t threadIdTotalX; // スレッドの総数X
	uint32_t threadIdOffsetY; // スレッドのオフセットY
	uint32_t threadIdTotalY; // スレッドの総数Y
	uint32_t threadIdOffsetZ; // スレッドのオフセットZ
	uint32_t threadIdTotalZ; // スレッドの総数Z
	float32_t4 clearColor; // クリアするときの色
	float32_t threshold; // しきい値
};

// 定数データ
ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);
// ソースR
Texture2D<float32_t4> sourceImageR : register(t0);
// ソースI
Texture2D<float32_t4> sourceImageI : register(t1);
// 行先R
RWTexture2D<float32_t4> destinationImageR : register(u0);
// 行先I
RWTexture2D<float32_t4> destinationImageI : register(u1);
// 行先R1
RWTexture2D<float32_t4> destinationImageR1 : register(u2);
// 行先I1
RWTexture2D<float32_t4> destinationImageI1 : register(u3);

void Copy(float32_t2 index) {

	destinationImageR[index] = sourceImageR[index];

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainCopy(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Copy(dispatchId.xy);

	}

}

void Clear(float32_t2 index) {

	destinationImageR[index] = gComputeConstants.clearColor;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainClear(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Clear(dispatchId.xy);

	}

}

void BinaryThreshold(float32_t2 index) {

	float32_t3 input = sourceImageR[index].rgb;

	float32_t3 col = float32_t3(0.0, 0.0, 0.0);

	if ((input.r + input.g + input.b) / 3.0 > gComputeConstants.threshold) {
		col = float32_t3(1.0, 1.0, 1.0);
	}

	destinationImageR[index] = float32_t4(col, 1.0f);

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBinaryThreshold(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		BinaryThreshold(dispatchId.xy);

	}

}