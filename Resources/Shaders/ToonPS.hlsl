#include "Object3D.hlsli"

Texture2D<float4> tex : register(t0); // -> 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // ------> 0番スロットに設定されたサンプラー

struct PSOutput
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
};

PSOutput main(VSOutput input)
{
    PSOutput output;
    
    // テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv) * input.color;
    
    // 光沢度
    const float shininess = 20.0f;
    
    // 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos - input.worldPos.xyz);
    
    // シェーディングによる色
    float4 shadecolor = float4(0.0f, 0.0f, 0.0f, m_alpha);
    
    // 平行光源
    for (int i = 0; i < DIRLIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
            // ライトに向かうベクトルと法線の内積
            float3 dotLightNormal = dot(dirLights[i].lightv, input.normal);

            // 反射光ベクトル
            float3 reflect = normalize(-dirLights[i].lightv + 2.0 * dotLightNormal * input.normal);
            
            // 拡散反射光
            float3 diffuse = dotLightNormal * m_diffuse;
            
            // 鏡面反射光
            float3 specular = step(0.4, pow(saturate(dot(reflect, eyedir)), shininess)) * m_specular;
            
            // 環境光
            float3 ambient = m_ambient * 0.3;

            // すべて加算する
            shadecolor.rgb += (diffuse + specular + ambient) * dirLights[i].lightcolor.rgb;
        }
    }
    
    // 点光源
    for (i = 0; i < POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
            // ライトへのベクトル
            float3 lightv = input.worldPos.rgb - pointLights[i].position;
            
            // ポイントライトまでの距離
            float3 distance = length(lightv);
            
            // 指数によるコントロール
            float3 factor = pow(saturate(-distance / pointLights[i].radius + 1.0), pointLights[i].decay);

            // 正規化し、単位ベクトルにする
            lightv = normalize(lightv);

            // ライトに向かうベクトルと法線の内積
            float3 dotLightNormal = dot(lightv, input.normal);

            // 反射光ベクトル
            float3 reflect = normalize(-lightv + 2.0 * dotLightNormal * input.normal);
            
            // 拡散反射光
            float3 diffuse = dotLightNormal * m_diffuse;

            // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
            
            // 環境光
            float3 ambient = m_ambient * 0.3;

            // すべてを加算する
            shadecolor.rgb += factor * (diffuse + specular + ambient) * pointLights[i].color.rgb;
        }
    }
    
    // スポットライト
    for (i = 0; i < SPOTLIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
            // ライトへの方向ベクトル
            float3 lightv = spotLights[i].lightpos - input.worldPos.xyz;
            float d = length(lightv);
            lightv = normalize(lightv);
        
            // 距離減衰係数
            float atten = saturate(1.0f / (spotLights[i].lightatten.x + spotLights[i].lightatten.y * d + spotLights[i].lightatten.z * d * d));
        
            // 角度減衰
            float cos = dot(lightv, spotLights[i].lightv);
        
            // 減衰開始角度から、減衰終了角度にかけて減衰
            // 減衰開始角度の内側は1倍減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
        
            // 角度減衰を乗算
            atten *= angleatten;
        
            // ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, input.normal);
            
            // 反射光ベクトル
            float3 reflect = normalize(-lightv + 2.0f * dotlightnormal * input.normal);
            
            // 拡散反射光
            float3 diffuse = dotlightnormal * m_diffuse;
            
            // 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

            // すべてを加算する
            shadecolor.rgb += atten * (diffuse + specular) * spotLights[i].lightcolor;
        }
    }
    
    // 丸影
    for (i = 0; i < CIRCLESHADOW_NUM; i++)
    {
        if (circleShadows[i].active)
        {
            //オブジェクト表面からキャスターへのベクトル
            float3 casterv = circleShadows[i].casterPos - input.worldPos.xyz;
            //投影方向での距離
            float d = dot(casterv, circleShadows[i].dir);
            //距離減衰係数
            float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
            //距離がマイナスなら0にする
            atten *= step(0, d);
            //仮想ライトの座標
            float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
            //オブジェクト表面からライトへのベクトル（単位ベクトル）
            float3 lightv = normalize(lightpos - input.worldPos.xyz);
            //角度減衰
            float cos = dot(lightv, circleShadows[i].dir);
            //減衰開始各おから、減衰終了角度にかけて減衰
            //減衰開始角度の内側は1倍、減衰終了角度の外側は0倍の輝度
            float angleatten =
            smoothstep(circleShadows[i].factorAngleCos.y,
            circleShadows[i].factorAngleCos.x, cos);
            //角度減衰を乗算
            atten *= angleatten;
            //すべて減算する
            shadecolor.rgb -= atten;
        }

    }

    // シェーディングによる色で描画
    output.target0 = shadecolor * texcolor;
    output.target1 = shadecolor * texcolor;
    return output;
}