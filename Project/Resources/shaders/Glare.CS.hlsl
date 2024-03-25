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
#define IMAGE 0

groupshared float3 BufferflyArray[2][2][LENGTH];
#define SharedArray(tmpID, x, realImage) (ButterflyArray[(tmpID)][(realImage)][(x)])

// fftSeries(バタフライウェイトパス)

