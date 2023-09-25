//////////////////////////////////////////
// Filename: textureshaderclass.cpp
//////////////////////////////////////////
#include "textureshaderclass.h"


TextureShaderClass::TextureShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}

TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// 정점 셰이더의 파일 이름을 설정한다.
	error = wcscpy_s(vsFilename, 128, L"Engine/texture.hlsl");
	if (error != 0)
	{
		return false;
	}

	// 픽셀 셰이더의 파일 이름을 설정한다.
	error = wcscpy_s(psFilename, 128, L"Engine/texture.hlsl");
	if (error != 0)
	{
		return false;
	}

	// 정점 및 픽셀 셰이더를 초기화한다.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextureShaderClass::Shutdown()
{
	// 정점 및 픽셀 셰이더를 관련된 오브젝트와 함께 할당 해제 및 정리한다.
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result; 

	
	// 렌더링에 사용할 셰이더 인자를 설정한다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// 이제 셰이더를 이용해 준비된 버퍼를 렌더링한다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3DBlob* errorMessage;
	ID3DBlob* vertexShaderBuffer;
	ID3DBlob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// 이 함수에서 사용할 포인터들을 null로 초기화한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 컴파일한다.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더가 컴파일에 실패하면 에러 메시지에 무언가 쓰여져 있을 것이다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 에러 메시지에 아무것도 쓰여진 게 없으면 셰이더 파일을 찾을 수 없는 것이다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 셰이더를 컴파일한다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더가 컴파일에 실패하면 에러 메시지에 무언가 쓰여져 있을 것이다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 에러 메시지에 아무것도 쓰여진 게 없으면 셰이더 파일을 찾을 수 없는 것이다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 셰이더 객체를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 입력 레이아웃 서술자(description)을 작성한다.
	// 해당 설정은 ModelClass의 VertexType 구조체와 셰이더의 VertexInputType이 일치하도록 해야 한다.
	polygonLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	polygonLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	// 레이아웃 요소의 개수를 구한다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 만든다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// 다 사용한 정점 셰이더 버퍼와 픽셀 셰이더 버퍼는 해제해준다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점 버퍼에 있는 동적 행렬 상수 버퍼에 대해 서술자(description)를 작성한다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 생성하여 해당 클래스 내에서 정점 셰이더 상수 버퍼에 접근할 수 있게 한다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 텍스처 샘플링 상태 서술자(description)을 생성한다.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 생성한다.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureShaderClass::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// 에러 메시지를 위한 텍스트 버퍼의 포인터를 획득한다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지 길이를 구한다.
	bufferSize = errorMessage->GetBufferSize();

	// 에러 메시지를 쓸 파일을 연다.
	fout.open("shader-error.txt");

	// 에러 메시지를 작성한다.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 파일을 닫는다.
	fout.close();

	// 에러 메시지를 해제한다.
	errorMessage->Release();
	errorMessage = 0;

	// 사용자에게 컴파일 에러가 적힌 텍스트 파일을 확인하도록 알리기 위해 메시지를 화면 상에 띄운다.
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;	
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// 셰이더에서 사용할 수 있게 행렬을 전치한다.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// 작성을 위해 상수 버퍼를 잠근다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 상수 버퍼의 데이터를 가리키는 포인터를 획득한다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 행렬을 상수 버퍼에 복사한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼에 대한 잠금을 해제한다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서 상수 버퍼의 위치를 표시한다.
	bufferNumber = 0;

	// 최종적으로 상수 버퍼를 정점 셰이더에 갱신된 값으로 기록한다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	
	// 셰이더 텍스처 리소스를 픽셀 셰이더에 설정한다.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형 렌더링에 사용할 정점 및 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 픽셀 셰이더에 샘플러 상태를 설정한다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState); // 이거 오류 날 수도 있다.

	// 삼각형을 렌더링한다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}