#include "PostEffectTest.hlsli"

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
    // tex0にブラーをかける
    // xyで2aの幅で色を取得
    float totalWeight = 0, _Sigma = 0.005, _StepWidth = 0.001;
    float4 col0 = float4(0, 0, 0, 0);

    for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth)
    {
        for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth)
        {
            float2 pickUV = input.uv + float2(px, py);
            float weight = Gaussian(input.uv, pickUV, _Sigma);

            // Gaussianで取得した「重み」を色にかける
            col0 += tex0.Sample(smp, pickUV) * weight;

            // かけた「重み」の合計値を控えておく
            totalWeight += weight;
        }
    }
    col0.rgb = col0.rgb / totalWeight; //かけた「重み」分、結果から割る
    col0.a = 1;
    
    float4 colorTex1 = tex1.Sample(smp, input.uv);

    float4 color = float4(1 - colorTex1.rgb, 1);
    
    if (fmod(input.uv.y, 0.1f) < 0.05f)
    {
        color = col0;
    }

    return float4(color.rgb, 1);
}