#pragma once
#include "ResourceBase.h"

class Texture : public ResourceBase
{
	using Super = ResourceBase;
public:
	Texture();
	~Texture();

	void CreateTextureFromResource(ComPtr<ID3D11Texture2D> tex2D);

	void Create(D3D11_TEXTURE2D_DESC desc);
	virtual void Load(const wstring& path) override;

	void SetSRV(D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc);
	void SetDSV(D3D11_DEPTH_STENCIL_VIEW_DESC desc);
	void SetRTV();
	void SetRTV(ComPtr<ID3D11RenderTargetView> rtv) { _rtv = rtv; }

	ComPtr<ID3D11Texture2D> GetTexture2D() { return _tex2D; }
	ComPtr<ID3D11ShaderResourceView> GetSRV() { return _srv; }
	ComPtr<ID3D11RenderTargetView> GetRTV() { return _rtv; }
	ComPtr<ID3D11DepthStencilView> GetDSV() { return _dsv; }
	ComPtr<ID3D11UnorderedAccessView> GetUAV() { return _uav; }

	Vec2 GetSize() { return _size; }

	const DirectX::ScratchImage& GetInfo() { return _img; }

private:
	ComPtr<ID3D11Texture2D>				_tex2D;
	ComPtr<ID3D11ShaderResourceView>	_srv;
	ComPtr<ID3D11RenderTargetView>		_rtv;
	ComPtr<ID3D11DepthStencilView>		_dsv;
	ComPtr<ID3D11UnorderedAccessView>	_uav;

	Vec2 _size = {0.f, 0.f};
	DirectX::ScratchImage _img = {};

	D3D11_TEXTURE2D_DESC _desc;
};

