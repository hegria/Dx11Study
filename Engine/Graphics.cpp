#include "pch.h"
#include "Graphics.h"

void Graphics::Init(HWND hwnd)
{
	_hwnd = hwnd;
	CreateDeviceAndSwapChain();
	CreateRenderTargetGroups();

	//CreateDeviceAndSwapChain();
	//CreateRenderTargetView();
	//CreateDepthStencilView();
	SetViewport(GAME->GetGameDesc().width, GAME->GetGameDesc().height);
}

void Graphics::ShadowRenderBegin()
{
	BindShadowRenderTarget();
}

void Graphics::RenderBegin()
{
	_rtGroups[0]->OMSetRenderTargets();
	_rtGroups[0]->ClearRenderTargetView();
	_vp.RSSetViewport();
}

void Graphics::RenderEnd()
{
	HRESULT hr = _swapChain->Present(1, 0);
	CHECK(hr);
}
void Graphics::CreateDeviceAndSwapChain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	{
		desc.BufferDesc.Width = GAME->GetGameDesc().width;
		desc.BufferDesc.Height = GAME->GetGameDesc().height;
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = _hwnd;
		desc.Windowed = TRUE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	}

	HRESULT hr = ::D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&desc,
		_swapChain.GetAddressOf(),
		_device.GetAddressOf(),
		nullptr,
		_deviceContext.GetAddressOf()
	);

	CHECK(hr);
}

void Graphics::CreateRenderTargetView()
{
	HRESULT hr;

	ComPtr<ID3D11Texture2D> backBuffer = nullptr;
	hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf());
	CHECK(hr);

	hr = _device->CreateRenderTargetView(backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
	CHECK(hr);
}

void Graphics::CreateDepthStencilView()
{
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = static_cast<uint32>(GAME->GetGameDesc().width);
		desc.Height = static_cast<uint32>(GAME->GetGameDesc().height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, _depthStencilTexture.GetAddressOf());
		CHECK(hr);
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = DEVICE->CreateDepthStencilView(_depthStencilTexture.Get(), &desc, _depthStencilView.GetAddressOf());
		CHECK(hr);
	}

}

void Graphics::CreateShadowMapView()
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = _vp.GetWidth();
	texDesc.Height = _vp.GetHeight();
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ComPtr<ID3D11Texture2D> depthMap;
	HRESULT hr = DEVICE->CreateTexture2D(&texDesc, 0, depthMap.GetAddressOf());
	CHECK(hr);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = DEVICE->CreateDepthStencilView(depthMap.Get(), &dsvDesc, _ShadowdepthMapDSV.GetAddressOf());
	CHECK(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	hr = DEVICE->CreateShaderResourceView(depthMap.Get(), &srvDesc, _ShadowdepthMapSRV.GetAddressOf());
	CHECK(hr);
}

void Graphics::BindShadowRenderTarget()
{
	_vp.RSSetViewport();

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	_deviceContext->OMSetRenderTargets(1, renderTargets, _ShadowdepthMapDSV.Get());
	_deviceContext->ClearDepthStencilView(_ShadowdepthMapDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Graphics::CreateRenderTargetGroups()
{
	D3D11_TEXTURE2D_DESC desc = { 0 };
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = static_cast<uint32>(GAME->GetGameDesc().width);
	desc.Height = static_cast<uint32>(GAME->GetGameDesc().height);
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	shared_ptr<Texture> dsTexture = make_shared<Texture>();
	dsTexture->Create(desc);
	RESOURCES->Add<Texture>(L"DepthStencil", dsTexture);



	D3D11_DEPTH_STENCIL_VIEW_DESC depthdesc;
	ZeroMemory(&depthdesc, sizeof(depthdesc));
	depthdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthdesc.Texture2D.MipSlice = 0;
	dsTexture->SetDSV(depthdesc);

	// SwapChain Group
	{
		vector<RenderTarget> rtVec(SWAP_CHAIN_BUFFER_COUNT);
		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			ComPtr<ID3D11Texture2D> resource;
			_swapChain->GetBuffer(i, __uuidof(ID3D11Texture2D), (void**)resource.GetAddressOf());
			shared_ptr<Texture> tex = make_shared<Texture>();
			tex->CreateTextureFromResource(resource);
			tex->SetRTV();
			RESOURCES->Add<Texture>(L"SwapChainTarget", tex);
			rtVec[i].target = tex;
		}
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)]->Create(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN, rtVec, dsTexture);

	}

	// Shadow Group
	
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT);

		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = _vp.GetWidth();
		texDesc.Height = _vp.GetHeight();
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		shared_ptr<Texture> shadowDepthTex = make_shared<Texture>();
		shadowDepthTex->Create(texDesc);
		RESOURCES->Add<Texture>(L"ShadowDepthStencil", shadowDepthTex);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Flags = 0;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
		shadowDepthTex->SetDSV(dsvDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		shadowDepthTex->SetSRV(srvDesc);


		shared_ptr<Texture> tempTex = make_shared<Texture>();
		ID3D11RenderTargetView* renderTargets = nullptr;
		ComPtr<ID3D11RenderTargetView> nullrtv = ComPtr<ID3D11RenderTargetView>(renderTargets);
		tempTex->SetRTV(nullrtv);
		rtVec[0].target = tempTex;
		RESOURCES->Add<Texture>(L"ShadowTarget", tempTex);


		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::SHADOW)]->Create(RENDER_TARGET_GROUP_TYPE::SHADOW, rtVec, shadowDepthTex);

	}
	// Deferred Group
	{
		vector<RenderTarget> rtVec(RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT);
		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = _vp.GetWidth();
		texDesc.Height = _vp.GetHeight();
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		shared_ptr<Texture> posTex = make_shared<Texture>();
		posTex->Create(texDesc);
		RESOURCES->Add<Texture>(L"PositionTarget", posTex);
		shared_ptr<Texture> normalTex = make_shared<Texture>();
		normalTex->Create(texDesc);
		RESOURCES->Add<Texture>(L"NormalTarget", normalTex);
		shared_ptr<Texture> DiffuseTex = make_shared<Texture>();
		DiffuseTex->Create(texDesc);
		RESOURCES->Add<Texture>(L"DiffuseTarget", DiffuseTex);
		rtVec[0].target = posTex;
		rtVec[1].target = normalTex;
		rtVec[2].target = DiffuseTex;

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
	}
	//Lighting Group

	{
		vector<RenderTarget> rtVec(RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT);

		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = _vp.GetWidth();
		texDesc.Height = _vp.GetHeight();
		texDesc.MipLevels = 1;
		texDesc.ArraySize = 1;
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		shared_ptr<Texture> DiffuseLightTarget = make_shared<Texture>();
		DiffuseLightTarget->Create(texDesc);
		RESOURCES->Add<Texture>(L"DiffuseLightTarget", DiffuseLightTarget);
		shared_ptr<Texture> SpecLightTarget = make_shared<Texture>();
		SpecLightTarget->Create(texDesc);
		RESOURCES->Add<Texture>(L"SpecularLightTarget", SpecLightTarget);
		rtVec[0].target = DiffuseLightTarget;
		rtVec[1].target = SpecLightTarget;

		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)] = make_shared<RenderTargetGroup>();
		_rtGroups[static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::G_BUFFER)]->Create(RENDER_TARGET_GROUP_TYPE::G_BUFFER, rtVec, dsTexture);
	}

	
}

void Graphics::SetViewport(float width, float height, float x /*= 0*/, float y /*= 0*/, float minDepth /*= 0*/, float maxDepth /*= 1*/)
{
	_vp.Set(width, height, x, y, minDepth, maxDepth);
}
