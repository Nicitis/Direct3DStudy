///////////////////////////////////
// Filename: colorshaderclass.cpp
///////////////////////////////////
#include "colorshaderclass.h"

ColorShaderClass::ColorShaderClass()
{
	m_vertexShader = nullptr;
	m_pixelShader = nullptr;
	m_layout = nullptr;
	m_matrixBuffer = nullptr;
	m_everyFrameChangedBuffer = nullptr;
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;
	wchar_t vsFilename[128];
	wchar_t psFilename[128];
	int error;

	// ���� ���̴��� ���� �̸��� �����Ѵ�.
	error = wcscpy_s(vsFilename, 128, L"Engine/color.hlsl");
	//error = wcscpy_s(vsFilename, 128, L"../Engine/color.vs");
	if (error != 0)
	{
		return false;
	}

	// �ȼ� ���̴��� ���� �̸��� �����Ѵ�.
	error = wcscpy_s(psFilename, 128, L"Engine/color.hlsl");
	//error = wcscpy_s(psFilename, 128, L"../Engine/color.ps");
	if (error != 0)
	{
		return false;
	}

	//  ���� �� �ȼ� ���̴��� �ʱ�ȭ�Ѵ�.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	// ���� ���̴��� �ȼ� ���̴��� ���õ� �͵�� �Բ� �����Ѵ�.
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float t)
{
	bool result;


	// �������� �ʿ��� ���̴��� ���ڸ� �����Ѵ�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, t);
	if (!result)
	{
		return false;
	}

	// ���̴��� �̿��Ͽ� �غ�� ���۸� �׸���.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc, everyFrameChangedBufferDesc;

	// �ش� �Լ����� ����� �����͸� null�� �ʱ�ȭ�Ѵ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���̴� ������ �������Ѵ�.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
			&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �����Ͽ� ������ ��� ���� �޽����� ���� ������ �־�� ��
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޽����� �ƹ��͵� ���ٸ� ���̴� ������ ã�� ���� ���̴�.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// �ȼ� ���̴��� �������Ѵ�.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// ���̴� �����Ͽ� ������ ��� ���� �޽����� ���� ������ �־�� ��
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// ���� �޽����� �ƹ��͵� ���ٸ� ���̴� ������ ã�� ���� ���̴�.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// ���۷κ��� ���� ���̴��� �����Ѵ�.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���۷κ��� �ȼ� ���̴��� �����Ѵ�.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// ���� �Է� ���̾ƿ� description�� �ۼ��Ѵ�.
	// �� ������ ModelClass�� �ش� ���̴��� �ִ� VertexType�� ������ ��ġ�ؾ� �Ѵ�.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// ���̾ƿ��� ��� ������ �����´�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]); // _countof ��ũ�θ� ����ص� �ȴ�

	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// ���̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� �Ҵ� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// ���� ���̴��� �ִ� ���� ��� ��� ������ description�� �ۼ��Ѵ�.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ���� ���� �����͸� �����Ͽ� �ش� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ֵ��� �Ѵ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �� ������ ����Ǵ� �����Ϳ� ���� ��� ������ description�� �ۼ��Ѵ�.
	everyFrameChangedBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	everyFrameChangedBufferDesc.ByteWidth = sizeof(EveryFrameChangedType);
	everyFrameChangedBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	everyFrameChangedBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	everyFrameChangedBufferDesc.MiscFlags = 0;
	everyFrameChangedBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&everyFrameChangedBufferDesc, nullptr, &m_everyFrameChangedBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void ColorShaderClass::ShutdownShader()
{
	// ��� ��� ���۸� �����Ѵ�.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_everyFrameChangedBuffer)
	{
		m_everyFrameChangedBuffer->Release();
		m_everyFrameChangedBuffer = 0;
	}
	// ���̾ƿ��� �����Ѵ�.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}
	// �ȼ� ���̴��� �����Ѵ�.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader= 0;
	}
	// ���� ���̴��� �����Ѵ�.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
	ofstream fout;


	// ���� �޽��� �ؽ�Ʈ ���ۿ� ���� �����͸� ȹ���Ѵ�.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// �޽��� ���̸� ���Ѵ�.
	bufferSize = errorMessage->GetBufferSize();

	// �����޽����� �ۼ��� ������ ����.
	fout.open("shader-error.txt");

	// ���� �޽����� �ۼ��Ѵ�.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// ������ �ݴ´�.
	fout.close();

	// ���� �޽����� �Ҵ� �����Ѵ�.
	errorMessage->Release();
	errorMessage = 0;

	// ȭ�鿡 �޽����� ��� �������� ������ ������ ���� �ؽ�Ʈ ������ Ȯ���ϵ��� �˸���.
	MessageBox(hwnd, L"���̴� ������ ������ �߻��߽��ϴ�. �޽��� Ȯ���� ���� shader-error.txt ������ �����ϼ���.", shaderFilename, MB_OK);

	return;
}


bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, float t)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	EveryFrameChangedType* everyFrameChangedDataPtr;

	// ���̴��� ������ �� �ְ� ����� ��ġ��Ų��
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	
	// ��� ��� ���ۿ� ����� �����Ѵ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); // ��� ���۸� �� �� �ֵ��� ��ٴ�.
	if (FAILED(result))
		return false;

	dataPtr = (MatrixBufferType*)mappedResource.pData; // ��� ���ۿ��� �����Ϳ� ���� �����͸� ȹ���Ѵ�.

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	//dataPtr->time = t;

	deviceContext->Unmap(m_matrixBuffer, 0); // ��� ������ ����� �����Ѵ�.

	// �� ������ ����Ǵ� ��� ������ ���� �����Ѵ�.
	result = deviceContext->Map(m_everyFrameChangedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); // ��� ���۸� �� �� �ֵ��� ��ٴ�.
	if (FAILED(result))
		return false;

	everyFrameChangedDataPtr = (EveryFrameChangedType*)mappedResource.pData;
	everyFrameChangedDataPtr->time = t;

	deviceContext->Unmap(m_everyFrameChangedBuffer, 0); // ��� ������ ����� �����Ѵ�.

	// ���������� ���̴��� ��� ������ ���� ���ŵ� ������ �����Ѵ�.
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &m_everyFrameChangedBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &m_everyFrameChangedBuffer);

	return true;
}


void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ�� �������� ���� ���� �� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ﰢ���� �������Ѵ�.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}