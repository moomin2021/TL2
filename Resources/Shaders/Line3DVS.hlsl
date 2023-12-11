#include "Line3D.hlsli"

VSOutput main(float4 pos : POSITION)
{
    VSOutput output;
    output.svPos = mul(mat, pos);
    return output;
}