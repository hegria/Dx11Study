#include "pch.h"
#include "Texture.h"

Texture::Texture() : Super(ResourceType::Texture)
{

}

Texture::~Texture()
{

}

void Texture::CreateTextureFromResource(ComPtr<ID3D11Texture2D> tex2D)
{
	_tex2D = tex2D;
	_tex2D->GetDesc(&_desc);

	if (_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = _desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = _desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		SetSRV(srvDesc);
	}
}

void Texture::Create(D3D11_TEXTURE2D_DESC desc)
{
	HRESULT hr = DEVICE->CreateTexture2D(&desc, nullptr, _tex2D.GetAddressOf());
	CHECK(hr);
}

void Texture::Load(const wstring& path)
{
	DirectX::TexMetadata md;
	HRESULT hr = ::LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &md, _img);
	CHECK(hr);

	hr = ::CreateShaderResourceView(DEVICE.Get(), _img.GetImages(), _img.GetImageCount(), md, _srv.GetAddressOf());
	CHECK(hr);
	
	_size.x = md.width;
	_size.y = md.height;
}

void Texture::SetSRV(D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc)
{
	HRESULT hr = DEVICE->CreateShaderResourceView(_tex2D.Get(), &srvDesc, _srv.GetAddressOf());
	CHECK(hr);
}

void Texture::SetDSV(D3D11_DEPTH_STENCIL_VIEW_DESC desc)
{
	HRESULT hr = DEVICE->CreateDepthStencilView(_tex2D.Get(), &desc, _dsv.GetAddressOf());
	CHECK(hr);
}

void Texture::SetRTV()
{
	HRESULT hr = DEVICE->CreateRenderTargetView(_tex2D.Get(), nullptr, _rtv.GetAddressOf());
	CHECK(hr);
}
