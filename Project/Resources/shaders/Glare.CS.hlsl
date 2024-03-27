#include "Complex.hlsl"

#include "Glare.hlsl"

struct ComputeParameters {
	float lamdaR;
	float lamdaG;
	float lamdaB;
	float glareIntensity;
	float thereshold;
};

ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);
Texture2D<float4> sourceImageR : register(t0);
Texture2D<float4> sourceImageI : register(t1);
RWTexture2D<float4> destinationImageR : register(u0);
RWTexture2D<float4> destinationImageI : register(u1);
RWTexture2D<float4> destinationImageR1 : register(u2);
RWTexture2D<float4> destinationImageI1 : register(u3);

// コピー
[numthreads(WIDTH, 1, 1)]
void mainCopy( uint3 dispatchId : SV_DispatchThreadID )
{

	float2 index = dispatchId.xy;

	destinationImageR[index] = sourceImageR[index];

}

// 白黒
[numthreads(WIDTH, 1, 1)]
void mainBinaryThreshold(uint3 dispatchId : SV_DispatchThreadID) 
{

	float2 index = dispatchId.xy;
	float3 input = sourceImageR[index].rgb;

	float3 col = float3(0.0, 0.0, 0.0);

	float r = 0;
	float g = 0;
	float b = 0;

	if ((input.r + input.g + input.b) / 3.0 > gComputeConstants.thereshold) {
		col = float3(1.0, 1.0, 1.0);
	}

	destinationImageR[index] = float4(col, 1.0f);

}

// クリア
[numthreads(WIDTH, 1, 1)]
void mainClear(uint3 dispatchId : SV_DispatchThreadID) {
	
	float2 index = dispatchId.xy;

	destinationImageR[index] = float4(0.0, 0.0, 0.0, 1.0);

}

// fftSeries(元データのID)
void ComputeSrcID(uint passIndex, uint x, out uint2 indices)
{

	uint regionWidth = 2u << passIndex;
	indices.x = (x & ~(regionWidth - 1u)) + (x & (regionWidth / 2u - 1u));
	indices.y = indices.x + regionWidth / 2;

	if (passIndex == 0) {
		indices = reversebits(indices) >> (32u - BUTTERFLY_COUNT) & (LENGTH - 1u);
	}

}

// fftSeries(回転因子)
void ComputeTwiddleFactor(uint passIndex, uint x, out uint2 weights) {

	uint regionWidth = 2u << passIndex;
	sincos(2.0 * PI * float(x & (regionWidth - 1u)) / float(regionWidth), weights.y, weights.x);
	weights.y *= -1;

}

#define REAL 0
#define IMAGE 1

#ifdef DOUBLE // メモリに余裕があるなら
groupshared float3 ButterflyArray[2][2][LENGTH];
#define SharedArray(tmpID, x, realImage) (ButterflyArray[(tmpID)][(realImage)][(x)])
#else
groupshared float3 ButterflyArray[2][LENGTH];
#define SharedArray(tmpID, x, realImage) (ButterflyArray[(realImage)][(x)])
#endif

// fftSeries(バタフライウェイトパス)
void ButterflyWeightPass(uint passIndex, uint x, uint tmp, out float3 resultR, out float3 resultI) {

	uint2 indices;
	float2 weights;

	ComputeSrcID(passIndex, x, indices);

	float3 inputR1 = SharedArray(tmp, indices.x, REAL);
	float3 inputI1 = SharedArray(tmp, indices.x, IMAGE);

	float3 inputR2 = SharedArray(tmp, indices.y, REAL);
	float3 inputI2 = SharedArray(tmp, indices.y, IMAGE);

	ComputeTwiddleFactor(passIndex, x, weights);

#ifndef DOUBLE
	GroupMemoryBarrierWithGroupSync();
#endif

#if INVERSE
	resultR = (inputR1 + weights.x * inputR2 + weights.y * inputI2) * 0.5;
	resultI = (inputI1 - weights.y * inputR2 + weights.x * inputI2) * 0.5;
#else
	resultR = inputR1 + weights.x * inputR2 - weights.y * inputI2;
	resultI = inputI1 + weights.y * inputR2 + weights.x * inputI2;
#endif

}

// fftSeries(FFT初期化共有配列)
void InitializeFFTSharedArray(uint bufferID, inout uint2 texPos) {

	texPos = (texPos + LENGTH / 2) % LENGTH;
	SharedArray(0, bufferID, REAL) = sourceImageR[texPos].xyz;

#if ROW && !INVERSE
	SharedArray(0, bufferID, IMAGE) = (0.0).xxx;
#else
	SharedArray(0, bufferID, IMAGE) = sourceImageI[texPos].xyz;
#endif

}

// fftSeries(バタフライパス)
void ButterflyPass(in uint bufferID, out float3 real, out float3 image) {

	for (int butterflyID = 0; butterflyID < BUTTERFLY_COUNT - 1; butterflyID++) {
		GroupMemoryBarrierWithGroupSync();
		ButterflyWeightPass(butterflyID, bufferID, butterflyID % 2,
			SharedArray((butterflyID + 1) % 2, bufferID, REAL),
			SharedArray((butterflyID + 1) % 2, bufferID, IMAGE));
	}

	GroupMemoryBarrierWithGroupSync();
	ButterflyWeightPass(BUTTERFLY_COUNT - 1, bufferID, (BUTTERFLY_COUNT - 1) % 2,
		real, image);

}

// FFTメイン
[numthreads(LENGTH, 1, 1)]
void mainFFT(uint3 dispatchId : SV_DispatchThreadID) {

	const uint bufferID = dispatchId.x;

#if ROW
	uint2 texPos = dispatchId.xy;
#else
	uint2 texPos = dispatchId.yx;
#endif

	InitializeFFTSharedArray(bufferID, texPos);

	float3 r_result = 0, i_result = 0;
	ButterflyPass(bufferID, r_result, i_result);

	destinationImageR[texPos].rgb = r_result;
	destinationImageI[texPos].rgb = i_result;

}

// 増幅
[numthreads(WIDTH, 1, 1)]
void mainAmp(uint3 dispatchId : SV_DispatchThreadID){

	float2 index = dispatchId.xy;
	float3 inputR = sourceImageR[index].rgb;
	float3 inputI = sourceImageI[index].rgb;

	float r = inputR.r * inputR.r + inputI.r * inputI.r;
	float g = inputR.g * inputR.g + inputI.g * inputI.g;
	float b = inputR.b * inputR.b + inputI.b * inputI.b;

	float3 col = float3(sqrt(r), sqrt(g), sqrt(b));

	destinationImageR[index] = float4(col, 1.0f);
	destinationImageI[index] = float4(col, 1.0f);

}


// 最大値最小値計算1回目
[numthreads(1, 1, 1)]
void mainMaxMinFirst(uint3 dispatchId : SV_DispatchThreadID) {

	uint2 index = dispatchId.xy;
	float3 colorMax = sourceImageR[float2(0, 0)].xyz;
	float3 colorMin = colorMax;

	int i;
	for (i = 0; i < HEIGHT; i++) {
		uint2 indexX = uint2(index.x, i);
		float3 color = sourceImageR[indexX].xyz;
	
		colorMax = max(colorMax, color);
		colorMin = min(colorMin, color);

	}

	destinationImageR[float2(index.x, 0)] = float4(colorMax, 1.0f);
	destinationImageR[float2(index.x, 1)] = float4(colorMin, 1.0f);

}

// 最大値最小値計算2回目
[numthreads(1, 1, 1)]
void mainMaxMinSecond(uint3 dispatchId : SV_DispatchThreadID) {

	uint2 index = dispatchId.xy;
	float3 colorMax = sourceImageR[float2(0, 0)].xyz;
	float3 colorMin = colorMax;

	int i;
	for (i = 0; i < HEIGHT; i++) {
		uint2 index1 = uint2(i, 0);
		uint2 index2 = uint2(i, 1);
		float3 colorMaxTmp = sourceImageR[index1].xyz;
		float3 colorMinTmp = sourceImageR[index2].xyz;

		colorMax = max(colorMax, colorMaxTmp);
		colorMin = min(colorMin, colorMinTmp);

	}

	destinationImageR[float2(0, 0)] = float4(colorMax, 1.0f);
	destinationImageI[float2(0, 0)] = float4(colorMin, 1.0f);

}

// 最大振幅による除算
[numthreads(WIDTH, 1, 1)]
void mainDivByMaxAmp(uint3 dispatchId : SV_DispatchThreadID) {

	uint2 index = dispatchId.xy;

	float2 zero = float2(0.0, 0.0);

	float3 colorMax = sourceImageR[zero].rgb;

	float3 colorR = destinationImageR[index].rgb;
	float3 colorI = destinationImageI[index].rgb;

	colorR = colorR / colorMax;
	colorI = colorI / colorMax;

	float colMaxPer = colorMax.r;
	colMaxPer = max(colMaxPer, colorMax.g);
	colMaxPer = max(colMaxPer, colorMax.b);

	float3 ratio = colorMax / colMaxPer;

	colorR = colorR * ratio;
	colorI = colorI * ratio;

	destinationImageR[index] = float4(colorR, 1.0f);
	destinationImageI[index] = float4(colorI, 1.0f);

}

// グレアの輝度を底上げ
[numthreads(WIDTH, 1, 1)]
void mainRaiseRealImage(uint3 dispatchId : SV_DispatchThreadID) {

	float2 index = dispatchId.xy;
	float3 inputR = sourceImageR[index].rgb;
	float3 inputI = sourceImageI[index].rgb;

	float3 amplitude = float3(
		sqrt(inputR.r * inputR.r + inputI.r * inputI.r),
		sqrt(inputR.g * inputR.g + inputI.g * inputI.g),
		sqrt(inputR.b * inputR.b + inputI.b * inputI.b));

	if ((amplitude.r + amplitude.g + amplitude.b) / 3.0f < 0.9) {
		inputR = inputR * gComputeConstants.glareIntensity;
		inputI = inputI * gComputeConstants.glareIntensity;
	}

	if ((inputR.r + inputR.g + inputR.b) / 3.0f >= 1.0) {
		inputR = float3(1.0, 1.0, 1.0);
	}

	if ((inputI.r + inputI.g + inputI.b) / 3.0f >= 1.0) {
		inputI = float3(1.0, 1.0, 1.0);
	}

	destinationImageR[index] = float4(inputR, 1.0f);
	destinationImageI[index] = float4(inputI, 1.0f);

}

// スペクトルスケール
[numthreads(WIDTH, 1, 1)]
void mainSpectrumScaling(uint3 dispatchId : SV_DispatchThreadID) {

	float2 indexR = dispatchId.xy;

	float ratioRG = gComputeConstants.lamdaG / gComputeConstants.lamdaR;
	float ratioRB = gComputeConstants.lamdaB / gComputeConstants.lamdaR;

	float2 uvR = indexR - float2(0.5 * WIDTH, 0.5 * HEIGHT);
	float2 uvG = uvR * ratioRG;
	float2 uvB = uvR * ratioRB;

	float2 indexG = uvG + float2(0.5 * WIDTH, 0.5 * HEIGHT);
	float2 indexB = uvB + float2(0.5 * WIDTH, 0.5 * HEIGHT);

	float r = sourceImageR[indexR].r;
	float g = sourceImageR[indexG].g;
	float b = sourceImageR[indexB].b;

	destinationImageR[indexR] = float4(r, g, b, 1.0);


	r = sourceImageI[indexR].r;
	g = sourceImageI[indexG].g;
	b = sourceImageI[indexB].b;

	destinationImageI[indexR] = float4(r, g, b, 1.0);

}

// 乗算
[numthreads(WIDTH, 1, 1)]
void mainMul(uint3 dispatchId : SV_DispatchThreadID){

	float2 index = dispatchId.xy;

	float3 colorReal1 = sourceImageR[index].rgb; // 元1 実部
	float3 colorImage1 = sourceImageI[index].rgb; // 元1 虚部

	float3 colorReal2 = destinationImageR[index].rgb; // 元2 実部
	float3 colorImage2 = destinationImageI[index].rgb; // 元2 虚部

	// 各色からの複素数
	float2 compR1 = float2(colorReal1.r, colorImage1.r);
	float2 compG1 = float2(colorReal1.g, colorImage1.g);
	float2 compB1 = float2(colorReal1.b, colorImage1.b);

	float2 compR2 = float2(colorReal2.r, colorImage2.r);
	float2 compG2 = float2(colorReal2.g, colorImage2.g);
	float2 compB2 = float2(colorReal2.b, colorImage2.b);

	// 各色毎に乗算
	float2 mulCompR = ComplexMul(compR1, compR2);
	float2 mulCompG = ComplexMul(compG1, compG2);
	float2 mulCompB = ComplexMul(compB1, compB2);

	// 代入用の複素数作成
	float3 colorReal = float3(mulCompR.x, mulCompG.x, mulCompB.x);
	float3 colorImage = float3(mulCompR.y, mulCompG.y, mulCompB.y);

	destinationImageR1[index] = float4(colorReal, 1.0f);
	destinationImageI1[index] = float4(colorImage, 1.0f);

}

// 加算
[numthreads(WIDTH, 1, 1)]
void mainAdd(uint3 dispatchId : SV_DispatchThreadID) {

	float2 index = dispatchId.xy;
	float3 input1 = sourceImageR[index].rgb;
	float3 input2 = sourceImageI[index].rgb;

	float3 col = input1 + input2;

	destinationImageR[index] = float4(col, 1.0f);

}

