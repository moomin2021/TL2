#include "PostEffectTest.hlsli"

VSOutput main( float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output;
    
    output.svPos = pos;
    output.uv = uv;
    return output;
}