#include "Complex.hlsl"

#include "Glare.hlsl"

struct ComputeParameters {
	float lamdaR;
	float lamdaG;
	float lamdaB;
	float glareIntensity;
	float thereshold;
};

ConstantBuffer<ComputeParameters> computeConstants : register(b0);
Texture2D<float4> sourceImageR : register(t0);
Texture2D<float4> sourceImageI : register(t1);
RWTexture2D<float4> destinationImageR : register(u0);
RWTexture2D<float4> destinationImageI : register(u1);
RWTexture2D<float4> destinationImageR1 : register(u2);
RWTexture2D<float4> destinationImageI1 : register(u3);

[numthreads(WIDTH, 1, 1)]
void mainCopy( uint3 dispatchId : SV_DispatchThreadID )
{

	float2 index = dispatchId.xy;

	destinationImageR[index] = sourceImageR[index];

}

[numthreads(WIDTH, 1, 1)]
void mainBinaryThreshold(uint3 dispatchId : SV_DispatchThreadID) 
{

	float2 index = dispatchId.xy;
	float3 input = sourceImageR[index].rgb;

	float3 col = float3(0.0, 0.0, 0.0);

	float r = 0;
	float g = 0;
	float b = 0;

	if ((input.r + input.g + input.b) / 3.0 > computeConstants.thereshold) {
		col = float3(1.0, 1.0, 1.0);
	}

	destinationImageR[index] = float4(col, 1.0f);

}

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

groupshared float3 ButterflyArray[2][2][LENGTH];
#define SharedArray(tmpID, x, realImage) (ButterflyArray[(tmpID)][(realImage)][(x)])

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
