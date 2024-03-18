RWStructuredBuffer<float> real : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 gID : SV_GroupID )
{

	real[gID.x] = gID.x;

}