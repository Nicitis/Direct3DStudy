////////////////////////////////////////////////////////////////////////////////
// Filename: d3dclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	// numerator�� ����, denominator�� �и� ����Ų��.
	unsigned int numModes, i, numerator, denominator;

	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	float fieldOfView, screenAspect;

	// vsync ������ �����Ѵ�..
	m_vsync_enabled = vsync;

	/* === ��� ===
	1. ���÷��� ���� ��������
	2. ���� ü��/��ġ/���� Ÿ�� �� �����ϱ�
	3. ����-���ٽ� ���� �����ϱ�
	4. ������ȭ ���� ����, ����Ʈ, ��ȯ ���
	*/

	////////////////////////////////////////////////////////////////////////////////
	// ���÷��� ���� ��������
	////////////////////////////////////////////////////////////////////////////////
	// DirectX �׷��� �������̽� ���丮�� �����Ѵ�.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// ���丮 ��ü�� ���� �⺻ �׷��� �������̽�(�׷��� ī��)�� ���� ����͸� �����Ѵ�.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// �ֿ� ����� ���(�����)�� �����Ѵ�.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM ���÷��� ���Ŀ� �´� ����� ��¿� ��带 ã�´�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// �ش� �����/�׷��� ī�� ���տ� ������ ��� ���÷��̸� ���� ��� ����Ʈ�� �����Ѵ�.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// ���� ���÷��� ��� ����Ʈ�� ä���.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ��� ���÷��� ��带 Ȯ���ϸ鼭 ȭ�� �ʺ�� ���̿� �´� ���� ã�´�.
	// ���ǿ� �´� ��带 ã���� ����� �ֻ����� ����/�и� ���� �����Ѵ�.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}


	// �׷��� ī�� ������� description�� ȹ���Ѵ�.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// �ش��ϴ� �׷��� ī���� �޸𸮸� MB ������ �����Ѵ�.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// �׷��� ī�� �̸��� ���� �迭�� ��ȯ�Ͽ� �����Ѵ�.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}


	// ���÷��� ��� ����Ʈ�� �����Ѵ�.
	delete[] displayModeList;
	displayModeList = 0;

	// ����� ����� �����Ѵ�.
	adapterOutput->Release();
	adapterOutput = 0;

	// ����͸� �����Ѵ�.
	adapter->Release();
	adapter = 0;

	// ���丮�� �����Ѵ�.
	factory->Release();
	factory = 0;

	////////////////////////////////////////////////////////////////////////////////
	// ���� ü��/��ġ/���� Ÿ�� �� �����ϱ�
	////////////////////////////////////////////////////////////////////////////////
	// ���� ü�� description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// �� ���۸� �ϳ��� �����Ѵ�.
	swapChainDesc.BufferCount = 1;

	// �� ������ �ʺ�� ���̸� �����Ѵ�.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// �� ���۸� �Ϲ� 32��Ʈ surface�� �����Ѵ�.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;


	// �� ������ �ֻ����� �����Ѵ�.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// �� ������ �뵵(usage)�� �����Ѵ�.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� �� �������� �ڵ� ���� �����Ѵ�.
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ���ø��� ��Ȱ��ȭ�Ѵ�.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ��üȭ������ â ������� �����Ѵ�.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// ��ĵ ���� �׸��� ������ �����ϸ��� �������� ����(unspecified)���� �����Ѵ�.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ȭ�鿡 �׸� �Ŀ� �� ���� ������ ������.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �÷��׸� �������� �ʴ´�.
	swapChainDesc.Flags = 0;


	// feature level�� DirectX 11�� �����Ѵ�.
	featureLevel = D3D_FEATURE_LEVEL_11_0;


	// ���� ü�ΰ� Direct3D ��ġ, Direct3D ��ġ ���ؽ�Ʈ�� �����Ѵ�.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}


	// �� ���� �����͸� ȹ���Ѵ�.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// �� ���� �����͸� �̿��Ͽ� ���� Ÿ�� �並 �����Ѵ�.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// �� ���� �����Ͱ� �ʿ� ���������Ƿ� �������ش�.
	backBufferPtr->Release();
	backBufferPtr = 0;


	////////////////////////////////////////////////////////////////////////////////
	// ����-���ٽ� ���� �����ϱ�
	////////////////////////////////////////////////////////////////////////////////
	// ���� ������ description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ������ description�� �����Ѵ�.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;


	// description�� �̿��� ���� ������ �ؽ�ó�� �������ش�.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// ���ٽ� ������ description�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���ٽ� ���¸� ���� description�� �����Ѵ�.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ��� �ո��� �� ���ٽ� ����
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ��� �޸��� �� ���ٽ� ����
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	// ���� ���ٽ� ���¸� �����Ѵ�.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}


	// ���� ���ٽ� ���� ������ �����Ѵ�.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);


	// ���� ���ٽ� �並 �ʱ�ȭ���ش�.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ���� ���ٽ� description�� �����Ѵ�.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �並 �����Ѵ�.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}


	// ���� Ÿ�� ��� ���� ���ٽ� ���۸� ��� ������ ���������ο� ���ε��Ѵ�.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	////////////////////////////////////////////////////////////////////////////////
	// ������ȭ ���� ����, ����Ʈ, ��ȯ ���
	////////////////////////////////////////////////////////////////////////////////
	// � �������� ��� �׸� �������� �����ϴ� ������ description�� �����Ѵ�.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// description�� �̿��Ͽ� ������ȭ ���� ������ �����Ѵ�.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ������ȭ ���� ������ �����Ѵ�.
	m_deviceContext->RSSetState(m_rasterState);


	// �������� ���� ����Ʈ�� �����Ѵ�.
	m_viewport.Width = (float)screenWidth;
	m_viewport.Height = (float)screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// ����Ʈ�� �����Ѵ�.
	m_deviceContext->RSSetViewports(1, &m_viewport);


	// ���� ����� �����Ѵ�.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D �������� ���� ���� ����� �����Ѵ�.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);


	// ���� ����� ���� ��ķ� �ʱ�ȭ�Ѵ�.
	m_worldMatrix = XMMatrixIdentity();


	// 2D �������� ���� ���� ���� ����� �����Ѵ�.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}


void D3DClass::Shutdown()
{
	// �����ϱ� ������ â���� �ٲ����� ������ ���� ü���� ������ �� ���ܰ� �߻��� �� �ִ�.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}


void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// ���۸� �ʱ�ȭ�� ������ �����Ѵ�.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// �� ���۸� ����.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// ���� ���۸� ����.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void D3DClass::EndScene()
{
	// �������� �Ϸ�Ǹ� �� ���۸� ȭ�鿡 �׸���.
	if (m_vsync_enabled)
	{
		// ȭ�� �ֻ����� ������Ų��.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// ȭ���� �ִ��� ���� �׸���.
		m_swapChain->Present(0, 0);
	}

	return;
}


ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}


void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}


void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}


void D3DClass::SetBackBufferRenderTarget()
{
	// ���� Ÿ�� ��� ���� ���ٽ� ���۸� ��� ������ ���������ο� ���ε��Ѵ�.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	return;
}


void D3DClass::ResetViewport()
{
	// ����Ʈ�� �����Ѵ�.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}