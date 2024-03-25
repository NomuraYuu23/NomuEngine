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

// fftSeries
void ComputeSrcID(uint passIndex, uint x, out uint2 indices)
{

	//uint regionWidth = 2 << passIndex;
	//indeces.x = (x & ~(regionWidth - 1)) + (x & (regionWidth / 2 - 1));
	//indeces.y = indeces.x + regionWidth / 2;

	//if (passIndex == 0) {
	//	indices = reversebits(indices) >> (32 - BUTTERFLY_COUNT) & (LENGTH - 1);
	//}

}


