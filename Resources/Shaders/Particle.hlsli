cbuffer cbuff0 : register(b0)
{
    matrix mat;
}

// 頂点シェーダーの出力構造体
struct VSOutput
{
    float4 svpos    : SV_POSITION;  // システム用頂点座標
    float scale : TEXCOORD;
};

// ジオメトリシェーダからピクセルシェーダへの出力
struct GSOutput
{
    float4 svpos    : SV_POSITION;  // システム用頂点座標
    float2 uv       : TEXCOORD;     // uv値
};