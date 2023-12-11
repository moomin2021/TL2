#include "HighLumi.hlsli"

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
SamplerState smp : register(s0);

struct PSOutput
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
};

PSOutput main(VSOutput input)
{
    PSOutput output;
    
    float4 col0 = tex0.Sample(smp, input.uv);
    float4 col1 = tex1.Sample(smp, input.uv);
    float grayScale = col0.r * 0.299 + col0.g * 0.587 + col0.b * 0.114;
    float extract = smoothstep(0.6, 0.9, grayScale);
    col0 *= extract;
    col0.a = 1;
    
    output.target0 = col0;
    output.target1 = col1;
    
    return output;
}