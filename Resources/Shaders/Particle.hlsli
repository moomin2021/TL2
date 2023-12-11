cbuffer cbuff0 : register(b0)
{
    matrix mat;
}

// ���_�V�F�[�_�[�̏o�͍\����
struct VSOutput
{
    float4 svpos    : SV_POSITION;  // �V�X�e���p���_���W
    float scale : TEXCOORD;
};

// �W�I���g���V�F�[�_����s�N�Z���V�F�[�_�ւ̏o��
struct GSOutput
{
    float4 svpos    : SV_POSITION;  // �V�X�e���p���_���W
    float2 uv       : TEXCOORD;     // uv�l
};