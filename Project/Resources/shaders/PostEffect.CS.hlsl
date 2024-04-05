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

// 編集するテクスチャ情報
struct EditTextureInformation {
	uint32_t top; // 上
	uint32_t bottom; // 下
	uint32_t left; // 左
	uint32_t right; // 右
};

// 定数データ
ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);
// ソース0
Texture2D<float32_t4> sourceImage0 : register(t0);
// ソース1
Texture2D<float32_t4> sourceImage1 : register(t1);
// 行先
RWTexture2D<float32_t4> destinationImage0 : register(u0);
// 編集するテクスチャ情報
RWStructuredBuffer<EditTextureInformation> gEditTextureInformation : register(u1);

void Copy(float32_t2 index) {

	destinationImage0[index] = sourceImage0[index];

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

	destinationImage0[index] = gComputeConstants.clearColor;

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

	float32_t3 input = sourceImage0[index].rgb;

	float32_t3 col = float32_t3(0.0, 0.0, 0.0);

	if ((input.r + input.g + input.b) / 3.0 > gComputeConstants.threshold) {
		col = float32_t3(1.0, 1.0, 1.0);
	}

	destinationImage0[index] = float32_t4(col, 1.0f);

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBinaryThreshold(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		BinaryThreshold(dispatchId.xy);

	}

}

float32_t Gauss(float32_t i, float32_t sigma) {

	return 1.0f / (2.0f * PI * sigma * sigma) * exp(-(i * i) / (2.0f * sigma * sigma));

}

void GaussianBlur(float32_t2 index, float32_t2 dir) {

	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };

	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	for (int32_t i = -gComputeConstants.kernelSize / 2; i < gComputeConstants.kernelSize / 2; i += 2) {

		// インデックス
		indexTmp = index;

		indexTmp.x += (float32_t(i) + 0.5f) * dir.x;
		indexTmp.y += (float32_t(i) + 0.5f) * dir.y;

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// outputに加算
		output += sourceImage0[indexTmp] * weight;

		// 重みの合計に加算
		weightSum += weight;
	}

	// 重みの合計分割る
	output *= (1.0f / weightSum);

	// 代入
	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		GaussianBlur(dispatchId.xy, float32_t2(1.0f, 0.0f));

	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainGaussianBlurVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		GaussianBlur(dispatchId.xy, float32_t2(0.0f, 1.0f));

	}

}

void Bloom(float32_t2 index, float32_t2 dir) {

	// 入力色
	float32_t4 input = { 0.0f,0.0f,0.0f,0.0f };
	
	// 出力色
	float32_t4 output = { 0.0f,0.0f,0.0f,0.0f };

	// 一時的なインデックス
	float32_t2 indexTmp = { 0.0f,0.0f };

	// 重み
	float32_t weight = 0.0f;

	// 重み合計
	float32_t weightSum = 0.0f;

	for (int32_t i = -gComputeConstants.kernelSize / 2; i < gComputeConstants.kernelSize / 2; i += 2) {

		// インデックス
		indexTmp = index;

		indexTmp.x += (float32_t(i) + 0.5f) * dir.x;
		indexTmp.y += (float32_t(i) + 0.5f) * dir.y;

		input = sourceImage0[indexTmp];

		// 重み確認
		weight = Gauss(float32_t(i), gComputeConstants.sigma) + Gauss(float32_t(i) + 1.0f, gComputeConstants.sigma);

		// 色確認
		if (((input.r + input.g + input.b) / 3.0 > gComputeConstants.threshold) || 
			(i <= 1 && i >= -1 )) {
			// outputに加算
			output += input * weight;
			// 重みの合計に加算
			weightSum += weight;
		}
	}


	// 重みの合計分割る
	output *= (1.0f / weightSum);

	// 代入
	destinationImage0[index] = output;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomHorizontal(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Bloom(dispatchId.xy, float32_t2(1.0f, 0.0f));

	}

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBloomVertical(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Bloom(dispatchId.xy, float32_t2(0.0f, 1.0f));

	}

}

void BrightnessThreshold(float32_t2 index) {

	float32_t4 input = sourceImage0[index];

	float32_t4 col = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

	if ((input.r + input.g + input.b) / 3.0f > gComputeConstants.threshold) {
		col = input;

		//gEditTextureInformation[0].top = min(gEditTextureInformation[0].top, int32_t(index.y));
		//gEditTextureInformation[0].bottom = max(gEditTextureInformation[0].bottom, int32_t(index.y));
		//gEditTextureInformation[0].left = min(gEditTextureInformation[0].left, int32_t(index.x));
		//gEditTextureInformation[0].right = max(gEditTextureInformation[0].right, int32_t(index.x));

	}

	destinationImage0[index] = col;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainBrightnessThreshold(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		BrightnessThreshold(dispatchId.xy);

	}

}

void Add(float32_t2 index) {

	float3 input1 = sourceImage0[index].rgb;
	float3 input2 = sourceImage1[index].rgb;

	float32_t alphaSum = sourceImage0[index].a + sourceImage1[index].a;
	float32_t a1 = sourceImage0[index].a / alphaSum;
	float32_t a2 = sourceImage1[index].a / alphaSum;

	float3 col = input1 * a1 + input2 * a2;

	destinationImage0[index] = float4(col, min(alphaSum, 1.0f));

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainAdd(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		Add(dispatchId.xy);

	}

}