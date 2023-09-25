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

	// ���� ���̴��� ���� �̸��� �����Ѵ�.
	error = wcscpy_s(vsFilename, 128, L"Engine/texture.hlsl");
	if (error != 0)
	{
		return false;
	}

	// �ȼ� ���̴��� ���� �̸��� �����Ѵ�.
	error = wcscpy_s(psFilename, 128, L"Engine/texture.hlsl");
	if (error != 0)
	{
		return false;
	}

	// ���� �� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextureShaderClass::Shutdown()
{
	// ���� �� �ȼ� ���̴��� ���õ� ������Ʈ�� �Բ� �Ҵ� ���� �� �����Ѵ�.
	ShutdownShader();

	return;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result; 

	
	// �������� ����� ���̴� ���ڸ� �����Ѵ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// ���� ���̴��� �̿��� �غ�� ���۸� �������Ѵ�.
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


	// �� �Լ����� ����� �����͵��� null�� �ʱ�ȭ�Ѵ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴��� �����Ͽ� �����ϸ� ���� �޽����� ���� ������ ���� ���̴�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޽����� �ƹ��͵� ������ �� ������ ���̴� ������ ã�� �� ���� ���̴�.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// �ȼ� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴��� �����Ͽ� �����ϸ� ���� �޽����� ���� ������ ���� ���̴�.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ���� �޽����� �ƹ��͵� ������ �� ������ ���̴� ������ ã�� �� ���� ���̴�.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// ���۷κ��� ���̴� ��ü�� �����Ѵ�.
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

	// ���� �Է� ���̾ƿ� ������(description)�� �ۼ��Ѵ�.
	// �ش� ������ ModelClass�� VertexType ����ü�� ���̴��� VertexInputType�� ��ġ�ϵ��� �ؾ� �Ѵ�.
	polygonLayout[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	polygonLayout[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	// ���̾ƿ� ����� ������ ���Ѵ�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// �� ����� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۴� �������ش�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���� ���ۿ� �ִ� ���� ��� ��� ���ۿ� ���� ������(description)�� �ۼ��Ѵ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� �����Ͽ� �ش� Ŭ���� ������ ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ؽ�ó ���ø� ���� ������(description)�� �����Ѵ�.
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

	// �ؽ�ó ���÷� ���¸� �����Ѵ�.
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


	// ���� �޽����� ���� �ؽ�Ʈ ������ �����͸� ȹ���Ѵ�.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽��� ���̸� ���Ѵ�.
	bufferSize = errorMessage->GetBufferSize();

	// ���� �޽����� �� ������ ����.
	fout.open("shader-error.txt");

	// ���� �޽����� �ۼ��Ѵ�.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// ������ �ݴ´�.
	fout.close();

	// ���� �޽����� �����Ѵ�.
	errorMessage->Release();
	errorMessage = 0;

	// ����ڿ��� ������ ������ ���� �ؽ�Ʈ ������ Ȯ���ϵ��� �˸��� ���� �޽����� ȭ�� �� ����.
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


	// ���̴����� ����� �� �ְ� ����� ��ġ�Ѵ�.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	// �ۼ��� ���� ��� ���۸� ��ٴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ��� ������ �����͸� ����Ű�� �����͸� ȹ���Ѵ�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ����� ��� ���ۿ� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ���ۿ� ���� ����� �����Ѵ�.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴����� ��� ������ ��ġ�� ǥ���Ѵ�.
	bufferNumber = 0;

	// ���������� ��� ���۸� ���� ���̴��� ���ŵ� ������ ����Ѵ�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	
	// ���̴� �ؽ�ó ���ҽ��� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ�� �������� ����� ���� �� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ȼ� ���̴��� ���÷� ���¸� �����Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState); // �̰� ���� �� ���� �ִ�.

	// �ﰢ���� �������Ѵ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}