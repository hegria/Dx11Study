#include "pch.h"
#include "RenderTargetGroup.h"

void RenderTargetGroup::Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture)
{
	_groupType = groupType;
	_rtVec = rtVec;
	_rtCount = static_cast<uint32>(rtVec.size());
	_depthStencilTexture = dsTexture;
	_depthStencilView = dsTexture->GetDSV();


	_renderTargetView.resize(_rtCount);

	for (uint32 i = 0; i < _rtCount; i++)
	{
		_renderTargetView[i] = _rtVec[i].target->GetRTV();
	}
}

void RenderTargetGroup::OMSetRenderTargets(uint32 count, uint32 offset)
{
	Viewport vp = Viewport(_rtVec[0].target->GetSize().x, _rtVec[0].target->GetSize().y);
	vp.RSSetViewport();
	GRAPHICS->GetDeviceContext()->OMSetRenderTargets(count, _renderTargetView[0].GetAddressOf(), _depthStencilView.Get());
}

void RenderTargetGroup::OMSetRenderTargets()
{
	Viewport vp = Viewport(_rtVec[0].target->GetSize().x, _rtVec[0].target->GetSize().y);
	vp.RSSetViewport();
	
	GRAPHICS->GetDeviceContext()->OMSetRenderTargets(_rtCount, _renderTargetView[0].GetAddressOf(), _depthStencilView.Get());
}

void RenderTargetGroup::ClearRenderTargetView(uint32 index)
{
	GRAPHICS->GetDeviceContext()->ClearRenderTargetView(_renderTargetView[index].Get(), (float*)(&GAME->GetGameDesc().clearColor));
	GRAPHICS->GetDeviceContext()->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void RenderTargetGroup::ClearRenderTargetView()
{
	for (uint32 i = 0; i < _rtCount; i++)
	{
		GRAPHICS->GetDeviceContext()->ClearRenderTargetView(_renderTargetView[i].Get(), (float*)(&GAME->GetGameDesc().clearColor));
	}
	GRAPHICS->GetDeviceContext()->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}
