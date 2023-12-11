#include "Particle.hlsli"

// �l�p�`�̒��_��
static const uint vnum = 4;

// �Z���^�[����̃I�t�Z�b�g
static const float4 offset_array[vnum] =
{
    float4(-10.0f, -10.0f, 0.0f, 0.0f), // ����
    float4(-10.0f, +10.0f, 0.0f, 0.0f), // ����
    float4(+10.0f, -10.0f, 0.0f, 0.0f), // �E��
    float4(+10.0f, +10.0f, 0.0f, 0.0f)  // �E��
};

// ���オ0, 0 �E����1, 1
static const float2 uv_array[vnum] =
{
    float2(0, 1), // ����
    float2(0, 0), // ����
    float2(1, 1), // �E��
    float2(1, 0)  // �E��
};

[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)

{
    GSOutput element;
	
	// 4�_���܂킷
    for (uint i = 0; i < vnum; i++)
    {
        float4 offset = offset_array[i] * input[0].scale;
        
        // ���[���h���W�x�[�X�ŁA���炷
        element.svpos = input[0].svpos + offset;
        
        // �r���[�A�ˉe�ϊ�
        element.svpos = mul(mat, element.svpos);
        element.uv = uv_array[i];
        output.Append(element);
    }

}