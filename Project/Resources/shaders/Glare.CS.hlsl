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