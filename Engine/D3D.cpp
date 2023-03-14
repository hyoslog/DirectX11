#include "D3D.h"

CD3D::CD3D()
	: bVsyncEnabled(false)
	, VideoCardMemory(0)
	, VideoCardDescription{}
	, SwapChain(nullptr)
	, Device(nullptr)
	, DeviceContext(nullptr)
	, RenderTargetView(nullptr)
	, DepthStencilBuffer(nullptr)
	, DepthStencilState(nullptr)
	, DepthStencilView(nullptr)
	, RasterState(nullptr)
	, ProjectionMatrix{}
	, WorldMatrix{}
	, OrthoMatrix{}
{
}

bool CD3D::Initialize(const unsigned int InScreenWidth, const int unsigned InScreenHeight, const bool InbVsync, const HWND InHwnd, const bool InbFullScreen, const float InScreenDepth, const float InScreenNear)
{
	HRESULT result = S_OK;

	// 수직 동기화 옵션 저장
	bVsyncEnabled = InbVsync;

	// DirectX 그래픽 인터페이스 팩토리 생성
	IDXGIFactory* factory = nullptr;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 팩터리를 사용하여 기본 그래픽 인터페이스(비디오 카드)용 어댑터 생성
	IDXGIAdapter* adapter = nullptr;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 기본 어댑터 출력(모니터)를 열거
	IDXGIOutput* adapterOutput = nullptr;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 어댑터 출력(모니터)에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 디스플레이 형식에 맞는 모드 수를 가져옴
	unsigned int modeCount = 0;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modeCount, nullptr);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 현재 모니터/비디오 카드 조합에 대해 가능한 모든 디스플레이 모드를 포함하는 목록을 생성
	DXGI_MODE_DESC* displayModes = nullptr;
	displayModes = new DXGI_MODE_DESC[modeCount];
	if (displayModes == nullptr)
	{
		return false;
	}

	// 디스플레이 모드 목록 구조를 채움
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modeCount, displayModes);
	if (FAILED(result) == true)
	{
		return false;
	}

	int numerator = 0; // 분자
	int denominator = 0; // 분모

	// 화면 너비 및 높이가 일치하는 모드를 찾고, 
	// 일치하는 항목이 있다면 해당 모니터에 대한 새로고침 빈도의 분자와 분모를 저장
	for (unsigned int modeIndex = 0; modeIndex < modeCount; ++modeIndex)
	{
		if ((displayModes[modeIndex].Width == InScreenWidth) && (displayModes[modeIndex].Height == InScreenHeight))
		{
			numerator = displayModes[modeIndex].RefreshRate.Numerator;
			denominator = displayModes[modeIndex].RefreshRate.Denominator;
		}
	}

	// 어댑터(비디오 카드) 설명을 가져옴
	DXGI_ADAPTER_DESC adapterDesc = {};
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 전용 비디오 카드 메모리를 메가바이트 단위로 저장
	VideoCardMemory = static_cast<unsigned int>((adapterDesc.DedicatedVideoMemory / (1024 * 1024)));

	// 비디오 카드의 이름을 문자 배열로 변환하여 저장
	unsigned long long stringLength = 0;
	const errno_t errorCode = wcstombs_s(&stringLength, VideoCardDescription, 128, adapterDesc.Description, 128);
	if (errorCode != 0)
	{
		return false;
	}

	// 디스플레이 모드 목록을 해제
	if (displayModes != nullptr)
	{
		delete[] displayModes;
		displayModes = nullptr;
	}

	// 어댑터 출력 해제
	if (adapterOutput != nullptr)
	{
		adapterOutput->Release();
		adapterOutput = nullptr;
	}

	// 어댑터 해제
	if (adapter != nullptr)
	{
		adapter->Release();
		adapter = nullptr;
	}

	// factory 해제
	if (factory != nullptr)
	{
		factory->Release();
		factory = nullptr;
	}

	// 스왑 체인 설명 초기화
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	// 단일 백 버퍼로 설정
	swapChainDesc.BufferCount = 1;

	// 백 버퍼의 너비와 높이 설정
	swapChainDesc.BufferDesc.Width = InScreenWidth;
	swapChainDesc.BufferDesc.Height = InScreenHeight;

	// 백 버퍼에 32비트 surface 설정
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백 버퍼의 재생 빈도 설정(백 버퍼를 프런트 버퍼로 그리는 초당 횟수)
	if (bVsyncEnabled == true)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백 버퍼의 용도 설정
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링할 창의 핸들 설정
	swapChainDesc.OutputWindow = InHwnd;

	// 멀티샘플링을 끔
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (InbFullScreen == true)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 스캔 라인 순서 및 스케일링을 지정되지 않음으로 설정
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 백 버퍼 내용 폐기
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

	// 기능 수준을 DX11로 설정
	const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑 체인, Direct3D 장치 및 Direct3D 장치 컨텍스트 생성
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &Device, nullptr, &DeviceContext);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 백 버퍼에 대한 포인터를 가져옴
	ID3D11Texture2D* backBuffer = nullptr;
	result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 스왑 체인에 백 버퍼를 연결
	result = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 백 버퍼 포인터 해제
	backBuffer->Release();
	backBuffer = nullptr;

	// 깊이 버퍼 설명을 설정하고, 스텐실 버퍼를 깊이 버퍼에 연결
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = InScreenWidth;
	depthBufferDesc.Height = InScreenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// 깊이/스텐실 버퍼 생성
	result = Device->CreateTexture2D(&depthBufferDesc, nullptr, &DepthStencilBuffer);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 깊이 스텐실 설명 설정
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// 전면 픽셀 스텐실 작업
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 후면 픽셀 스텐실 작업
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이 스텐실 상태 생성
	result = Device->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 생성된 깊이 스텐실 상태를 적용
	DeviceContext->OMSetDepthStencilState(DepthStencilState, 1);

	// 깊이 스텐실 뷰 설명 설정
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이 스텐실 뷰 생성
	result = Device->CreateDepthStencilView(DepthStencilBuffer, &depthStencilViewDesc, &DepthStencilView);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 렌더 타겟 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	// 래스터라이저 설명 설정
	D3D11_RASTERIZER_DESC rasterDesc = {};
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

	// 래스터라이저 상태 생성
	result = Device->CreateRasterizerState(&rasterDesc, &RasterState);
	if (FAILED(result) == true)
	{
		return false;
	}

	// 래스터라이즈 상태 설정
	DeviceContext->RSSetState(RasterState);

	// 뷰포트 설정
	D3D11_VIEWPORT viewPort = {};
	viewPort.Width = static_cast<float>(InScreenWidth);
	viewPort.Height = static_cast<float>(InScreenHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	// 뷰포트 생성
	DeviceContext->RSSetViewports(1, &viewPort);

	// 투영 행렬 설정
	float fielwOfView = (3.141592654f / 4.0f);
	float screenAspect = (static_cast<float>(InScreenWidth) / static_cast<float>(InScreenHeight));

	// 투영 행렬
	ProjectionMatrix = XMMatrixPerspectiveFovLH(fielwOfView, screenAspect, InScreenNear, InScreenDepth);

	// 월드 행렬 초기화
	WorldMatrix = XMMatrixIdentity();

	// 직교 행령
	OrthoMatrix = XMMatrixOrthographicLH(static_cast<float>(InScreenWidth), static_cast<float>(InScreenHeight), InScreenNear, InScreenDepth);

    return true;
}

void CD3D::Shutdown()
{
	if (SwapChain != nullptr)
	{
		SwapChain->SetFullscreenState(false, nullptr);
	}

	if (RasterState != nullptr)
	{
		RasterState->Release();
		RasterState = nullptr;
	}

	if (DepthStencilView != nullptr)
	{
		DepthStencilView->Release();
		DepthStencilView = nullptr;
	}

	if (DepthStencilState != nullptr)
	{
		DepthStencilState->Release();
		DepthStencilState = nullptr;
	}

	if (DepthStencilBuffer != nullptr)
	{
		DepthStencilBuffer->Release();
		DepthStencilBuffer = nullptr;
	}

	if (RenderTargetView != nullptr)
	{
		RenderTargetView->Release();
		RenderTargetView = nullptr;
	}

	if (DeviceContext != nullptr)
	{
		DeviceContext->Release();
		DeviceContext = nullptr;
	}

	if (Device != nullptr)
	{
		Device->Release();
		Device = nullptr;
	}

	if (SwapChain != nullptr)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}

	return;
}

void CD3D::BegineScene(const float InRed, const float InGreen, const float InBlue, const float InAlpha)
{
	float color[4] = {};

	// 백 버퍼를 지울 색상 설정
	color[0] = InRed;
	color[1] = InGreen;
	color[2] = InBlue;
	color[3] = InAlpha;

	// 백 버퍼를 지움
	DeviceContext->ClearRenderTargetView(RenderTargetView, color);

	// 깊이 버퍼를 지움
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void CD3D::EndScene()
{
	// 렌더링이 완료되었으므로 백 버퍼를 화면에 표시

	if(bVsyncEnabled == true)
	{
		// 화면 새로 고침 빈도로 고정
		SwapChain->Present(1, 0);
	}
	else
	{
		SwapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* CD3D::GetDevice() const
{
    return Device;
}

ID3D11DeviceContext* CD3D::GetDeviceContext() const
{
    return DeviceContext;
}

void CD3D::GetProjectionMatrix(XMMATRIX& OutProjectionMatrix)
{
	OutProjectionMatrix = ProjectionMatrix;
}

void CD3D::GetWorldMatrix(XMMATRIX& OutWorldMatrix)
{
	OutWorldMatrix = WorldMatrix;
}

void CD3D::GetOrthoMatrix(XMMATRIX& OutOrthoMatrix)
{
	OutOrthoMatrix = OrthoMatrix;
}

void CD3D::GetVideoCardInfo(char* OutCardName, int& OutMemory)
{
	strcpy_s(OutCardName, 128, VideoCardDescription);
	OutMemory = VideoCardMemory;
}
