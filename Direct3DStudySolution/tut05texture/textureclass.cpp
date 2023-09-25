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

	// targa 이미지 데이터를 메모리에 불러온다.
	result = LoadTarga32Bit(filename);
	if (!result)
	{
		MessageBox(NULL, L"LoadTarga32Bit", L"error", MB_OK);

		return false;
	}

	// 텍스처에 대한 서술자를 작성한다.
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
	
	// 빈 텍스처를 생성한다.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(hResult))
	{
		MessageBox(NULL, L"CreateTexture2D", L"error", MB_OK);
		return false;
	}

	// targa 이미지 파일에 대한 row pitch를 설정한다.
	rowPitch = (m_width * 4) * sizeof(unsigned char);

	// targa 이미지 데이터를 텍스처에 복사한다.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);
	
	// 셰이더 리소스 뷰 서술자(description)를 설정한다.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	// 텍스처에 대한 셰이더 리소스 뷰를 생성한다.
	hResult = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	if (FAILED(hResult))
	{
		MessageBox(NULL, L"CreateShaderResourceView", L"error", MB_OK);
		return false;
	}

	// 텍스처의 밉맵을 생성한다.
	deviceContext->GenerateMips(m_textureView);

	// 이미지 데이터가 텍스처에 불러오기되었으므로 targa 이미지 데이터를 해제한다.
	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	// 텍스처 뷰 자원(resource)를 해제한다.
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	// 텍스처를 해제한다.
	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	// targa 데이터를 해제한다.
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

	
	// targa 파일을 이진 읽기 모드로 연다.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot open file", L"error", MB_OK);
		return false;
	}

	// 파일 헤더를 연다.
	count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot open header", L"error", MB_OK);
		return false;
	}

	// 주요 정보들을 헤더로부터 취득한다
	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	bpp = (int)targaFileHeader.bpp;
	
	// 32비트이고 24비트가 아니라는 걸 확인하자.
	if (bpp != 32)
	{
		MessageBox(NULL, L"LoadTarga32Bit: bpp is not 32", L"error", MB_OK);

		return false;
	}

	// 32 비트 이미지 데이터의 크기를 계산한다.
	imageSize = m_width * m_height * 4;

	// targa 이미지 파일을 위한 메모리를 할당한다.
	targaImage = new unsigned char[imageSize];

	// targa 이미지 데이터를 읽어온다.
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot read from targa file", L"error", MB_OK);

		return false;
	}

	// 파일을 닫는다.
	error = fclose(filePtr);
	if (error != 0)
	{
		MessageBox(NULL, L"LoadTarga32Bit: Cannot close file", L"error", MB_OK);

		return false;
	}

	// targa 결과 데이터를 위해 메모리를 할당한다.
	m_targaData = new unsigned char[imageSize];

	// targa 결과 데이터 배열의 인덱스를 초기화한다.
	index = 0;

	// targa 이미지 데이터에 대한 인덱스를 초기화해준다.
	k = m_width * (m_height - 1) * 4;
	
	// 이제 targa 이미지 데이터를 targa 결과 배열에 올바른 순서로 복사한다(targa 형식은 위에서 아래로 저장되며 RGBA 순서가 아니기 때문).
	for (i = 0; i < m_height; i++)
	{
		for (j = 0; j < m_width; j++)
		{
			// Targa는 BGRA 순서로 저장
			m_targaData[index + 0] = targaImage[k + 2]; // 빨강
			m_targaData[index + 1] = targaImage[k + 1]; // 초록
			m_targaData[index + 2] = targaImage[k + 0]; // 파랑
			m_targaData[index + 3] = targaImage[k + 3]; // 알파(투명도)

			// 인덱스를 증가시킨다
			k += 4;
			index += 4;
		}
		// 지금은 거꾸로 읽고 있으니 targa 이미지 데이터를 이전 행의 시작 열로 이동시킨다.
		k -= (m_width * 8);		
	}

	// 복사가 끝났으니 targa 이미지 데이터를 삭젷나다.
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