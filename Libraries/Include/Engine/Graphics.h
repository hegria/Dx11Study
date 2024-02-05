#pragma once
#include "Viewport.h"
#include "RenderTargetGroup.h"

class Graphics
{
	DECLARE_SINGLE(Graphics);

public:
	void Init(HWND hwnd);

	void ShadowRenderBegin();

	void RenderBegin();
	void RenderEnd();

	ComPtr<ID3D11Device> GetDevice() { return _device; }
	ComPtr<ID3D11DeviceContext> GetDeviceContext() { return _deviceContext; }

private:
	void CreateDeviceAndSwapChain();
	void CreateRenderTargetView();
	void CreateDepthStencilView();
	void CreateShadowMapView();

	void BindShadowRenderTarget();

	void CreateRenderTargetGroups();

public:
	void SetViewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);
	Viewport& GetViewport() { return _vp; }

private:
	HWND _hwnd = {};

	// Device & SwapChain
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _deviceContext = nullptr;
	ComPtr<IDXGISwapChain> _swapChain = nullptr;

	// SwapChain
	ComPtr<ID3D11RenderTargetView> _renderTargetView;
	ComPtr<ID3D11Texture2D> _depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;

	// Shadow
	ComPtr<ID3D11RenderTargetView> _ShadowrenderTargetView;
	ComPtr<ID3D11ShaderResourceView> _ShadowdepthMapSRV;
	ComPtr<ID3D11DepthStencilView> _ShadowdepthMapDSV;

	// SSAO

	array<shared_ptr<RenderTargetGroup>, RENDER_TARGET_GROUP_COUNT> _rtGroups;

	// Misc
	//D3D11_VIEWPORT _viewport = { 0 };
	Viewport _vp;
};

