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
	int32_t kernelSize; // カーネルサイズ
	float32_t sigma; // 標準偏差
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

float32_t Gauss(float32_t x, float32_t y, float32_t sigma) {

	return 1.0f / (2.0f * PI * sigma * sigma) * exp(-(x * x + y * y) / (2.0f * sigma * sigma));

}

void GaussianBlur(float32_t2 index) {

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };
	
	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	for (int32_t x = -gComputeConstants.kernelSize / 2; x < gComputeConstants.kernelSize / 2; ++x) {

		for (int32_t y = -gComputeConstants.kernelSize / 2; y < gComputeConstants.kernelSize / 2; ++y) {

			// インデックス
			indexTmp = index;
			
			indexTmp.x += float32_t(x);
			//if (indexTmp.x < 0.0f || indexTmp.x > gComputeConstants.threadIdTotalX) {
			//	continue;
			//}
			
			indexTmp.y += float32_t(y);
			//if (indexTmp.y < 0.0f || indexTmp.y > gComputeConstants.threadIdTotalY) {
			//	continue;
			//}

			// 重み確認
			weight = Gauss(float32_t(x), float32_t(y), gComputeConstants.sigma);

			// outputに加算
			output += sourceImageR[indexTmp] * weight;

			// 重みの合計に加算
			weightSum += weight;

		}
	}

	// 重みの合計分割る
	output *= (1.0f / weightSum);

	// 代入
	destinationImageR[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlur(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		GaussianBlur(dispatchId.xy);

	}

}