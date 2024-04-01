#include "PostEffect.hlsli"

// 定数データ
struct ComputeParameters {
	// しきい値
	float thereshold;
};

// 定数データ
ConstantBuffer<ComputeParameters> gComputeConstants : register(b0);
// ソースR
Texture2D<float4> sourceImageR : register(t0);
// ソースI
Texture2D<float4> sourceImageI : register(t1);
// 行先R
RWTexture2D<float4> destinationImageR : register(u0);
// 行先I
RWTexture2D<float4> destinationImageI : register(u1);
// 行先R1
RWTexture2D<float4> destinationImageR1 : register(u2);
// 行先I1
RWTexture2D<float4> destinationImageI1 : register(u3);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}