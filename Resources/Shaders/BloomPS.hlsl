#include "Bloom.hlsli"

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
SamplerState smp : register(s0);

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
    float totalWeight = 0, _Sigma = 0.005, _StepWidth = 0.001;
    float4 col0 = float4(0, 0, 0, 0);

                // xyで2αの幅で色を取得
    for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth)
    {
        for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth)
        {
            float2 pickUV = clamp(input.uv + float2(px, py), 0.01, 0.99);
            float weight = Gaussian(input.uv, pickUV, _Sigma);

                        // Gaussianで取得した「重み」を色にかける
            col0 += tex0.Sample(smp, pickUV) * weight;

                        // かけた「重み」の合計値を控えておく
            totalWeight += weight;
        }
    }

    // かけた「重み」分、結果かた割る
    col0.rgb = col0.rgb / totalWeight;
    
    float4 col1 = tex1.Sample(smp, input.uv);
    return col0 + col1;
}