cbuffer ConstBufferData3D : register(b0)
{
    matrix viewproj;// --> �r���[�v���W�F�N�V�����s��
    matrix world;// -----> ���[���h�s��
    float3 cameraPos;// -> �J�������W(���[���h���W)
    float4 color;// -----> �F(RGBA)
}

cbuffer MaterialData : register(b1)
{
    float3 m_ambient : packoffset(c0); // --> �A���r�G���g�W��
    float3 m_diffuse : packoffset(c1); // --> �f�B�t�F�[�Y�W��
    float3 m_specular : packoffset(c2); // -> �X�y�L�����[�W��
    float m_alpha : packoffset(c2.w); // ---> �A���t�@
}

static const int DIRLIGHT_NUM = 1;
static const int POINTLIGHT_NUM = 10;
static const int SPOTLIGHT_NUM = 10;
static const int CIRCLESHADOW_NUM = 1;

struct DirLight
{
    float3 lightv; // -> ���C�g�ւ̕����̒P�ʃx�N�g��
    float3 lightcolor; // -> ���C�g�̐F(RGB)
    uint active;
};

struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
    float radius;
    float decay;
    uint active;
};

struct SpotLight
{
    float3 lightv; // -> ���C�g�̌��������̋t�x�N�g��
    float3 lightpos; // -> ���C�g���W
    float3 lightcolor; // -> ���C�g�̐F(RGB)
    float3 lightatten; // -> ���C�g���������W��
    float2 lightfactoranglecos; // -> ���C�g�����p�x�̃R�T�C��
    uint active;
};

struct CircleShadow
{
    float3 dir; // -> ���e�����t�x�N�g��(�P�ʃx�N�g��)
    float3 casterPos; // -> �L���X�^�[���W
    float distanceCasterLight; // -> �L���X�^�[�ƃ��C�g�̋���
    float3 atten; // -> ���������W��
    float2 factorAngleCos; // -> �����p�x�̃R�T�C��
    uint active;
};

cbuffer cbuff2 : register(b2)
{
    float3 ambientColor;
    DirLight dirLights[DIRLIGHT_NUM];
    PointLight pointLights[POINTLIGHT_NUM];
    SpotLight spotLights[SPOTLIGHT_NUM];
    CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// --���_�V�F�[�_�[�̏o�͍\����-- //
// --���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����-- //
struct VSOutput
{
    float4 svPos : SV_POSITION; // -> �V�X�e���p���_���W
    float4 worldPos : POSITION; // -> ���[���h���W
    float3 normal : NORMAL; // -----> �@��
    float4 color : COLOR;// --------> �F(RGBA)
    float2 uv : TEXCOORD;// --------> UV�l
};