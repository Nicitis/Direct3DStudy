////////////////////////////////////////////////////////////////////////////////
// Filename: applicationclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "applicationclass.h"
//#include <cstdio>


ApplicationClass::ApplicationClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Direct3D 객체를 생성하고 초기화한다.
	m_Direct3D = std::make_unique<D3DClass>();

	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not intialize Direct3D", L"Error", MB_OK);
		return false;
	}
	
	// 카메라 객체를 생성한다.
	m_Camera = std::make_unique<CameraClass>();

	// 카메라의 초기 위치를 설정한다.
	//m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->SetPosition(0.0f, 0.0f, -15.0f);

	// 모델 객체를 생성 및 초기화한다.
	m_Model = std::make_unique<ModelClass>();

	result = m_Model->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// 컬러 셰이더 객체를 생성 및 초기화한다.
	m_ColorShader = std::make_unique<ColorShaderClass>();

	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// 과제: 텍스트 파일에 그래픽 카드 쓰기
	//WriteVideoCardToFile("Video Card Info.txt");

	return true;
}


void ApplicationClass::Shutdown()
{
	// 컬러 셰이더 객체를 해제한다.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		m_ColorShader = 0;
	}

	// 모델 객체를 해제한다.
	if (m_Model)
	{
		m_Model->Shutdown();
		m_Model = 0;
	}

	// 카메라 객체를 해제한다.
	if (m_Camera)
	{
		m_Camera = 0;
	}

	// Direct3D 객체를 할당 해제한다.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		m_Direct3D = 0;
	}

	return;
}


bool ApplicationClass::Frame()
{
	bool result;

	// 그래픽스 씬을 렌더링한다.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool ApplicationClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// 버퍼를 비워 씬을 시작한다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// 현재 카메라의 위치를 통해 뷰 행렬을 생성한다.
	m_Camera->Render();

	// 월드, 뷰, 투영 행렬을 카메라와 d3d 객체로부터 획득한다.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// 테스트: 월드 행렬을 회전시킨다.
	static float t = 0.0f;
	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.0f;
	

	XMMATRIX spin = XMMatrixRotationY(-t);
	XMMATRIX translation = XMMatrixTranslation(3.0f, 0.0f, 0.0f);
	XMFLOAT3 axis(-1.0f, 1.0f, -1.0f);
	XMVECTOR vectorAxis = XMVector3Normalize(XMLoadFloat3(&axis));
	XMMATRIX orbit = XMMatrixRotationAxis(vectorAxis, -1.0f * t);
	worldMatrix = worldMatrix * spin * translation * orbit;

	// 모델 정점 및 인덱스 버퍼를 그래픽스 파이프라인에 넣어 그릴 준비를 한다.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// 컬러 셰이더를 사용해 모델을 렌더링한다.
	result = m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
	{
		return false;
	}

	// 렌더링된 씬을 화면 상에 전달한다.
	m_Direct3D->EndScene();

	return true;
}


// 튜토리얼 3 과제 4: 그래픽 카드의 이름과 메모리를 파일에 출력한다.
//bool ApplicationClass::WriteVideoCardToFile(const char* fileName)
//{
//	FILE* fp;
//	errno_t result = fopen_s(&fp, fileName, "w");
//	if (result != 0)
//	{
//		return false;
//	}
//	char videoCardName[128];
//	int memory;
//	m_Direct3D->GetVideoCardInfo(videoCardName, memory);
//	
//	fprintf(fp, "Video card information\nvideo card name: %s\ndedicated card memory: %d MB",
//		videoCardName, memory);
//	
//	return true;
//}