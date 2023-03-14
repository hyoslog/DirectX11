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

	// ���� ����ȭ �ɼ� ����
	bVsyncEnabled = InbVsync;

	// DirectX �׷��� �������̽� ���丮 ����
	IDXGIFactory* factory = nullptr;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ���͸��� ����Ͽ� �⺻ �׷��� �������̽�(���� ī��)�� ����� ����
	IDXGIAdapter* adapter = nullptr;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result) == true)
	{
		return false;
	}

	// �⺻ ����� ���(�����)�� ����
	IDXGIOutput* adapterOutput = nullptr;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ����� ���(�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORM ���÷��� ���Ŀ� �´� ��� ���� ������
	unsigned int modeCount = 0;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modeCount, nullptr);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ���� �����/���� ī�� ���տ� ���� ������ ��� ���÷��� ��带 �����ϴ� ����� ����
	DXGI_MODE_DESC* displayModes = nullptr;
	displayModes = new DXGI_MODE_DESC[modeCount];
	if (displayModes == nullptr)
	{
		return false;
	}

	// ���÷��� ��� ��� ������ ä��
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &modeCount, displayModes);
	if (FAILED(result) == true)
	{
		return false;
	}

	int numerator = 0; // ����
	int denominator = 0; // �и�

	// ȭ�� �ʺ� �� ���̰� ��ġ�ϴ� ��带 ã��, 
	// ��ġ�ϴ� �׸��� �ִٸ� �ش� ����Ϳ� ���� ���ΰ�ħ ���� ���ڿ� �и� ����
	for (unsigned int modeIndex = 0; modeIndex < modeCount; ++modeIndex)
	{
		if ((displayModes[modeIndex].Width == InScreenWidth) && (displayModes[modeIndex].Height == InScreenHeight))
		{
			numerator = displayModes[modeIndex].RefreshRate.Numerator;
			denominator = displayModes[modeIndex].RefreshRate.Denominator;
		}
	}

	// �����(���� ī��) ������ ������
	DXGI_ADAPTER_DESC adapterDesc = {};
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ���� ���� ī�� �޸𸮸� �ް�����Ʈ ������ ����
	VideoCardMemory = static_cast<unsigned int>((adapterDesc.DedicatedVideoMemory / (1024 * 1024)));

	// ���� ī���� �̸��� ���� �迭�� ��ȯ�Ͽ� ����
	unsigned long long stringLength = 0;
	const errno_t errorCode = wcstombs_s(&stringLength, VideoCardDescription, 128, adapterDesc.Description, 128);
	if (errorCode != 0)
	{
		return false;
	}

	// ���÷��� ��� ����� ����
	if (displayModes != nullptr)
	{
		delete[] displayModes;
		displayModes = nullptr;
	}

	// ����� ��� ����
	if (adapterOutput != nullptr)
	{
		adapterOutput->Release();
		adapterOutput = nullptr;
	}

	// ����� ����
	if (adapter != nullptr)
	{
		adapter->Release();
		adapter = nullptr;
	}

	// factory ����
	if (factory != nullptr)
	{
		factory->Release();
		factory = nullptr;
	}

	// ���� ü�� ���� �ʱ�ȭ
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	// ���� �� ���۷� ����
	swapChainDesc.BufferCount = 1;

	// �� ������ �ʺ�� ���� ����
	swapChainDesc.BufferDesc.Width = InScreenWidth;
	swapChainDesc.BufferDesc.Height = InScreenHeight;

	// �� ���ۿ� 32��Ʈ surface ����
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// �� ������ ��� �� ����(�� ���۸� ����Ʈ ���۷� �׸��� �ʴ� Ƚ��)
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

	// �� ������ �뵵 ����
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� â�� �ڵ� ����
	swapChainDesc.OutputWindow = InHwnd;

	// ��Ƽ���ø��� ��
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

	// ��ĵ ���� ���� �� �����ϸ��� �������� �������� ����
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// �� ���� ���� ���
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = 0;

	// ��� ������ DX11�� ����
	const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ���� ü��, Direct3D ��ġ �� Direct3D ��ġ ���ؽ�Ʈ ����
	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &Device, nullptr, &DeviceContext);
	if (FAILED(result) == true)
	{
		return false;
	}

	// �� ���ۿ� ���� �����͸� ������
	ID3D11Texture2D* backBuffer = nullptr;
	result = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ���� ü�ο� �� ���۸� ����
	result = Device->CreateRenderTargetView(backBuffer, nullptr, &RenderTargetView);
	if (FAILED(result) == true)
	{
		return false;
	}

	// �� ���� ������ ����
	backBuffer->Release();
	backBuffer = nullptr;

	// ���� ���� ������ �����ϰ�, ���ٽ� ���۸� ���� ���ۿ� ����
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

	// ����/���ٽ� ���� ����
	result = Device->CreateTexture2D(&depthBufferDesc, nullptr, &DepthStencilBuffer);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ���� ���ٽ� ���� ����
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// ���� �ȼ� ���ٽ� �۾�
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ĸ� �ȼ� ���ٽ� �۾�
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���ٽ� ���� ����
	result = Device->CreateDepthStencilState(&depthStencilDesc, &DepthStencilState);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ������ ���� ���ٽ� ���¸� ����
	DeviceContext->OMSetDepthStencilState(DepthStencilState, 1);

	// ���� ���ٽ� �� ���� ����
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �� ����
	result = Device->CreateDepthStencilView(DepthStencilBuffer, &depthStencilViewDesc, &DepthStencilView);
	if (FAILED(result) == true)
	{
		return false;
	}

	// ���� Ÿ�� ��� ���� ���ٽ� ���۸� ��� ���� ���������ο� ���ε�
	DeviceContext->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	// �����Ͷ����� ���� ����
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

	// �����Ͷ����� ���� ����
	result = Device->CreateRasterizerState(&rasterDesc, &RasterState);
	if (FAILED(result) == true)
	{
		return false;
	}

	// �����Ͷ����� ���� ����
	DeviceContext->RSSetState(RasterState);

	// ����Ʈ ����
	D3D11_VIEWPORT viewPort = {};
	viewPort.Width = static_cast<float>(InScreenWidth);
	viewPort.Height = static_cast<float>(InScreenHeight);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0.0f;
	viewPort.TopLeftY = 0.0f;

	// ����Ʈ ����
	DeviceContext->RSSetViewports(1, &viewPort);

	// ���� ��� ����
	float fielwOfView = (3.141592654f / 4.0f);
	float screenAspect = (static_cast<float>(InScreenWidth) / static_cast<float>(InScreenHeight));

	// ���� ���
	ProjectionMatrix = XMMatrixPerspectiveFovLH(fielwOfView, screenAspect, InScreenNear, InScreenDepth);

	// ���� ��� �ʱ�ȭ
	WorldMatrix = XMMatrixIdentity();

	// ���� ���
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

	// �� ���۸� ���� ���� ����
	color[0] = InRed;
	color[1] = InGreen;
	color[2] = InBlue;
	color[3] = InAlpha;

	// �� ���۸� ����
	DeviceContext->ClearRenderTargetView(RenderTargetView, color);

	// ���� ���۸� ����
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void CD3D::EndScene()
{
	// �������� �Ϸ�Ǿ����Ƿ� �� ���۸� ȭ�鿡 ǥ��

	if(bVsyncEnabled == true)
	{
		// ȭ�� ���� ��ħ �󵵷� ����
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
