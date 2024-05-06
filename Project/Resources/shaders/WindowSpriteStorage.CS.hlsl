#include "WindowSpriteStorage.hlsli"

// 定数データ
struct ComputeParameters {

	uint32_t threadIdOffsetX; // スレッドのオフセットX
	uint32_t threadIdTotalX; // スレッドの総数X
	uint32_t threadIdOffsetY; // スレッドのオフセットY
	uint32_t threadIdTotalY; // スレッドの総数Y
	uint32_t threadIdOffsetZ; // スレッドのオフセットZ
	uint32_t threadIdTotalZ; // スレッドの総数Z

	float32_t4 clearColor; // クリアするときの色

	uint32_t overwriteCount; // 上書き回数

};

// 定数データ
ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);

// ソース0
Texture2D<float32_t4> sourceImage0 : register(t0);
// ソース1
Texture2D<float32_t4> sourceImage1 : register(t1);
// ソース2
Texture2D<float32_t4> sourceImage2 : register(t2);
// ソース3
Texture2D<float32_t4> sourceImage3 : register(t3);
// ソース4
Texture2D<float32_t4> sourceImage4 : register(t4);
// ソース5
Texture2D<float32_t4> sourceImage5 : register(t5);
// ソース6
Texture2D<float32_t4> sourceImage6 : register(t6);
// ソース7
Texture2D<float32_t4> sourceImage7 : register(t7);

// 行先
RWTexture2D<float32_t4> destinationImage0 : register(u0);

float32_t4 Copy(in const float32_t4 input) {

	float32_t4 clear = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

	if (input.r == gComputeConstants.clearColor.r &&
		input.g == gComputeConstants.clearColor.g &&
		input.b == gComputeConstants.clearColor.b &&
		input.a == gComputeConstants.clearColor.a) {
		return clear;
	}

	return input;

}

// コピー
[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainCopy(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 input = sourceImage0[dispatchId.xy];
		destinationImage0[dispatchId.xy] = Copy(input);

	}

}

// 画像の上に画像を書き込む
float32_t4 Overwrite(in const float32_t4 input0, in const float32_t4 input1) {

	if (input1.a == 1.0f) {
		return input1;
	}
	else if (input1.a == 0.0f) {
		return input0;
	}

	float32_t alphaOut = 1.0f - input1.a;

	alphaOut = min(alphaOut, input0.a);

	if (input0.a != 0.0f) {
		float32_t a = min(alphaOut / input0.a, 1.0f);

		float32_t4 color =
			float32_t4(
				input0.r * a,
				input0.g * a,
				input0.b * a,
				alphaOut);

		return input1 + color;

	}

	return input1;

}

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void mainOverwrite(uint32_t3 dispatchId : SV_DispatchThreadID)
{

	if (dispatchId.x < gComputeConstants.threadIdTotalX &&
		dispatchId.y < gComputeConstants.threadIdTotalY) {

		float32_t4 output = float32_t4(0.0f,0.0f,0.0f,0.0f);
		float32_t4 input = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);

		float32_t4 source[8];
		source[0] = sourceImage0[dispatchId.xy];
		source[1] = sourceImage1[dispatchId.xy];
		source[2] = sourceImage2[dispatchId.xy];
		source[3] = sourceImage3[dispatchId.xy];
		source[4] = sourceImage4[dispatchId.xy];
		source[5] = sourceImage5[dispatchId.xy];
		source[6] = sourceImage6[dispatchId.xy];
		source[7] = sourceImage7[dispatchId.xy];

		for (uint32_t i = 0; i < gComputeConstants.overwriteCount; ++i) {
			float32_t4 input = source[i];
			output = Overwrite(output, input);
		}

		destinationImage0[dispatchId.xy] = output;

	}

}