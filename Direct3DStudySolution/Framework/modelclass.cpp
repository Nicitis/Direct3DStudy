////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


void ModelClass::Shutdown()
{
	// 정점 및 인덱스 버퍼를 해제한다.
	ShutdownBuffers();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 그리기 과정을 위해 정점 및 인덱스 버퍼를 그래픽스 파이프라인에 넣는다.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// 정점 배열에서 정점의 수를 설정한다.
	m_vertexCount = 3;

	// 인덱스 배열에서 인덱스의 수를 설정한다.
	m_indexCount = 3;

	// 정점 배열을 생성한다.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 생성한다.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열에 값을 넣는다.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // 왼쪽 아래
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // 위쪽 가운데
	vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // 오른쪽 아래
	vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	// 인덱스 배열에 값을 넣는다.
	indices[0] = 0; // 왼쪽 아래
	indices[1] = 1; // 위쪽 가운데
	indices[2] = 2; // 오른쪽 아래

	// 정적 정점 버퍼의 description을 설정한다.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조체에 정점 데이터에 대한 포인터를 전달한다.
	vertexData.pSysMem = vertices;	// 포인터를 통해 정점 배열을 초기화
	vertexData.SysMemPitch = 0;		// 텍스처(2D, 3D 한정)에서 다음 줄이 시작될 때까지의 간격
	vertexData.SysMemSlicePitch = 0;// 3D 텍스처에서 다음 깊이 레벨이 시작될 때까지의 간격

	// 이제 정점 배열을 생성한다
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정적(static) 인덱스 버퍼에 대한 description을 작성한다
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조체에 인덱스 데이터에 대한 포인터를 전달한다
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성한다
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 및 인덱스 버퍼를 만들고 값을 할당했으므로 배열들은 제거해준다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// 정점 버퍼의 단위(stride)와 오프셋을 설정한다.
	stride = sizeof(VertexType);
	offset = 0;

	// 입력 어셈블러에 정점 버퍼를 활성화하여 렌더링될 수 있게 한다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 입력 어셈블리에 인덱스 버퍼를 활성화하여 렌더링될 수 있게 한다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형(primitive)을 설정한다. 이 경우는 삼각형이 되겠다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}