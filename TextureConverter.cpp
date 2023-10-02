#include "TextureConverter.h"

void TextureConverter::ConvertTextureWICToDDS(const std::string& filePath)
{
	// �e�N�X�`���t�@�C����ǂݍ���
	LoadWICTextureFromFile(filePath);

#pragma region DDS�`���ɕϊ����ď����o��

#pragma endregion
}

void TextureConverter::LoadWICTextureFromFile(const std::string& filePath)
{
	// �t�@�C���p�X�����C�h������ɕϊ�����
	std::wstring wFilePath = ConvertMultiByteStringToWideString(filePath);

#pragma region �e�N�X�`����ǂݍ���
	// WIC�e�N�X�`���̃��[�h
	HRESULT result = DirectX::LoadFromWICFile(wFilePath.c_str(), DirectX::WIC_FLAGS_NONE, &metaData_, scratchImage_);
	assert(SUCCEEDED(result));
#pragma endregion
}

std::wstring TextureConverter::ConvertMultiByteStringToWideString(const std::string& mString)
{
	// ���C�h������ɕϊ������ۂ̕��������v�Z
	uint16_t filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, nullptr, 0);

	// ���C�h������
	std::wstring wString;
	wString.resize(filePathBufferSize);

	// ���C�h������ɕϊ�
	MultiByteToWideChar(CP_ACP, 0, mString.c_str(), -1, &wString[0], filePathBufferSize);

	return wString;
}

void TextureConverter::SeparateFilePath(const std::wstring& filePath)
{
	size_t pos1;
	std::wstring exceptExt;

	// ��؂蕶��'.'���o�Ă����ԍŌ�̕���������
	pos1 = filePath.rfind('.');

	// �������q�b�g������
	if (pos1 != std::wstring::npos) {
		// ��؂蕶���̌����t�@�C���g���q�Ƃ��ĕۑ�
		fileExt_ = filePath.substr(pos1 + 1, filePath.size() - pos1 - 1);

		// ��؂蕶���̑O�܂ł𔲂��o��
		exceptExt = filePath.substr(0, pos1);
	}

	else {
		fileExt_ = L"";
		exceptExt = filePath;
	}
}