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

	// 정점 셰이더의 파일 이름을 설정한다.
	error = wcscpy_s(vsFilename, 128, L"Engine/color.hlsl");
	//error = wcscpy_s(vsFilename, 128, L"../Engine/color.vs");
	if (error != 0)
	{
		return false;
	}

	// 픽셀 셰이더의 파일 이름을 설정한다.
	error = wcscpy_s(psFilename, 128, L"Engine/color.hlsl");
	//error = wcscpy_s(psFilename, 128, L"../Engine/color.ps");
	if (error != 0)
	{
		return false;
	}

	//  정점 및 픽셀 셰이더를 초기화한다.
	result = InitializeShader(device, hwnd, vsFilename, psFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ColorShaderClass::Shutdown()
{
	// 정점 셰이더와 픽셀 셰이더를 관련된 것들과 함께 종료한다.
	ShutdownShader();

	return;
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, 
		XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, float t)
{
	bool result;


	// 렌더링에 필요한 셰이더의 인자를 설정한다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, t);
	if (!result)
	{
		return false;
	}

	// 셰이더를 이용하여 준비된 버퍼를 그린다.
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

	// 해당 함수에서 사용할 포인터를 null로 초기화한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더 파일을 컴파일한다.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, 
			&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일에 실패할 경우 에러 메시지에 무언가 쓰여져 있어야 함
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 에러 메시지에 아무것도 없다면 셰이더 파일을 찾지 못한 것이다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// 픽셀 셰이더를 컴파일한다.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일에 실패할 경우 에러 메시지에 무언가 쓰여져 있어야 함
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 에러 메시지에 아무것도 없다면 셰이더 파일을 찾지 못한 것이다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼로부터 픽셀 셰이더를 생성한다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 입력 레이아웃 description을 작성한다.
	// 이 설정은 ModelClass와 해당 셰이더에 있는 VertexType의 구조가 일치해야 한다.
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

	// 레이아웃의 요소 갯수를 가져온다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]); // _countof 매크로를 사용해도 된다

	// 정점 입력 레이아웃을 생성한다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// 더이상 사용되지 않는 정점 셰이더 버퍼와 픽셀 셰이더 버퍼를 할당 해제한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점 셰이더에 있는 동적 행렬 상수 버퍼의 description을 작성한다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 정점 버퍼 포인터를 생성하여 해당 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있도록 한다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 매 프레임 변경되는 데이터에 대한 상수 버퍼의 description을 작성한다.
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
	// 행렬 상수 버퍼를 해제한다.
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
	// 레이아웃을 해제한다.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}
	// 픽셀 셰이더를 해제한다.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader= 0;
	}
	// 정점 셰이더를 해제한다.
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


	// 에러 메시지 텍스트 버퍼에 대한 포인터를 획득한다.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// 메시지 길이를 구한다.
	bufferSize = errorMessage->GetBufferSize();

	// 에러메시지를 작성할 파일을 연다.
	fout.open("shader-error.txt");

	// 에러 메시지를 작성한다.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// 파일을 닫는다.
	fout.close();

	// 에러 메시지를 할당 해제한다.
	errorMessage->Release();
	errorMessage = 0;

	// 화면에 메시지를 띄어 유저에게 컴파일 에러에 대한 텍스트 파일을 확인하도록 알린다.
	MessageBox(hwnd, L"셰이더 컴파일 오류가 발생했습니다. 메시지 확인을 위해 shader-error.txt 파일을 참고하세요.", shaderFilename, MB_OK);

	return;
}


bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
	XMMATRIX projectionMatrix, float t)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	EveryFrameChangedType* everyFrameChangedDataPtr;

	// 셰이더에 전달할 수 있게 행렬을 전치시킨다
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	
	// 행렬 상수 버퍼에 행렬을 복사한다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); // 상수 버퍼를 쓸 수 있도록 잠근다.
	if (FAILED(result))
		return false;

	dataPtr = (MatrixBufferType*)mappedResource.pData; // 상수 버퍼에서 데이터에 대한 포인터를 획득한다.

	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	//dataPtr->time = t;

	deviceContext->Unmap(m_matrixBuffer, 0); // 상수 버퍼의 잠금을 해제한다.

	// 매 프레임 변경되는 상수 버퍼의 값을 갱신한다.
	result = deviceContext->Map(m_everyFrameChangedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource); // 상수 버퍼를 쓸 수 있도록 잠근다.
	if (FAILED(result))
		return false;

	everyFrameChangedDataPtr = (EveryFrameChangedType*)mappedResource.pData;
	everyFrameChangedDataPtr->time = t;

	deviceContext->Unmap(m_everyFrameChangedBuffer, 0); // 상수 버퍼의 잠금을 해제한다.

	// 마지막으로 셰이더에 상수 버퍼의 값을 갱신된 값으로 설정한다.
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &m_everyFrameChangedBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &m_everyFrameChangedBuffer);

	return true;
}


void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형 렌더링에 사용될 정점 및 픽셀 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 삼각형을 렌더링한다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}