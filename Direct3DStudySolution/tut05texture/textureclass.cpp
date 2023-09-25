////////////////////////////////////////////////////////////////////////////////
// Filename: textureclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "textureclass.h"

TextureClass::TextureClass()
{
	m_targaData = 0;
	m_texture = 0;
	m_textureView = 0;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;
	int height, width;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	// targa �̹��� �����͸� �޸𸮿� �ҷ��´�.
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		MessageBox(NULL, L"LoadTarga32Bit", L"error", MB_OK);

		return false;
	}

	// �ؽ�ó�� ���� �����ڸ� �ۼ��Ѵ�.
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	
	// �� �ؽ�ó�� �����Ѵ�.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		MessageBox(NULL, L"CreateTexture2D", L"error", MB_OK);
		return false;
	}

	// targa �̹��� ���Ͽ� ���� row pitch�� �����Ѵ�.
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// targa �̹��� �����͸� �ؽ�ó�� �����Ѵ�.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);
	
	// ���̴� ���ҽ� �� ������(description)�� �����Ѵ�.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// �ؽ�ó�� ���� ���̴� ���ҽ� �並 �����Ѵ�.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		MessageBox(NULL, L"CreateShaderResourceView", L"error", MB_OK);
		return false;
	}

	// �ؽ�ó�� �Ӹ��� �����Ѵ�.
	deviceContext->GenerateMips(m_textureView);

	// �̹��� �����Ͱ� �ؽ�ó�� �ҷ�����Ǿ����Ƿ� targa �̹��� �����͸� �����Ѵ�.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// �ؽ�ó �� �ڿ�(resource)�� �����Ѵ�.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// �ؽ�ó�� �����Ѵ�.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa �����͸� �����Ѵ�.
	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga32Bit(char* filename)
{
	int error, bpp, imageSize, index, i, j, k;
	FILE* filePtr;
	unsigned int count;
	TargaHeader targaFileHeader;
	unsigned char* targaImage;

	
	// targa ������ ���� �б� ���� ����.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot open file", L"error", MB_OK);
		return false;
	}

	// ���� ����� ����.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot open header", L"error", MB_OK);
		return false;
	}

	// �ֿ� �������� ����κ��� ����Ѵ�
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;
	
	// 32��Ʈ�̰� 24��Ʈ�� �ƴ϶�� �� Ȯ������.
	if (bpp != 32)
	{
		MessageBox(NULL, L"LoadTarga32Bit: bpp is not 32", L"error", MB_OK);

		return false;
	}

	// 32 ��Ʈ �̹��� �������� ũ�⸦ ����Ѵ�.
	imageSize = m_width * m_height * 4;

	// targa �̹��� ������ ���� �޸𸮸� �Ҵ��Ѵ�.
	targaImage = new unsigned char[imageSize];

	// targa �̹��� �����͸� �о�´�.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot read from targa file", L"error", MB_OK);

		return false;
	}

	// ������ �ݴ´�.
	error = fclose(filePtr);
	if (error != 0)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot close file", L"error", MB_OK);

		return false;
	}

	// targa ��� �����͸� ���� �޸𸮸� �Ҵ��Ѵ�.
	m_targaData = new unsigned char[imageSize];

	// targa ��� ������ �迭�� �ε����� �ʱ�ȭ�Ѵ�.
	index = 0;

	// targa �̹��� �����Ϳ� ���� �ε����� �ʱ�ȭ���ش�.
	k = m_width * (m_height - 1) * 4;
	
	// ���� targa �̹��� �����͸� targa ��� �迭�� �ùٸ� ������ �����Ѵ�(targa ������ ������ �Ʒ��� ����Ǹ� RGBA ������ �ƴϱ� ����).
	for (i = 0; i < m_height; i++)
	{
		for (j = 0; j < m_width; j++)
		{
			// Targa�� BGRA ������ ����
			m_targaData[index + 0] = targaImage[k + 2]; // ����
			m_targaData[index + 1] = targaImage[k + 1]; // �ʷ�
			m_targaData[index + 2] = targaImage[k + 0]; // �Ķ�
			m_targaData[index + 3] = targaImage[k + 3]; // ����(����)

			// �ε����� ������Ų��
			k += 4;
			index += 4;
		}
		// ������ �Ųٷ� �а� ������ targa �̹��� �����͸� ���� ���� ���� ���� �̵���Ų��.
		k -= (m_width * 8);		
	}

	// ���簡 �������� targa �̹��� �����͸� �蠫����.
	delete[] targaImage;
	targaImage = 0;

	return true;
}

int TextureClass::GetWidth()
{
	return m_width;
}

int TextureClass::GetHeight()
{
	return m_height;
}