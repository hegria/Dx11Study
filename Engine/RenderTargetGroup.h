#pragma once


enum class RENDER_TARGET_GROUP_TYPE : uint8
{
	SWAP_CHAIN, // BACK_BUFFER, FRONT_BUFFER
	SHADOW, // SHADOW
	//SSAO,  // SSAO
	G_BUFFER, // POSITION, NORMAL, COLOR
	LIGHTING, // DIFFUSE LIGHT, SPECULAR LIGHT	
	END,
};

enum
{
	RENDER_TARGET_SHADOW_GROUP_MEMBER_COUNT = 1,
	RENDER_TARGET_SSAO_GROUP_MEMBER_COUNT = 1,
	RENDER_TARGET_G_BUFFER_GROUP_MEMBER_COUNT = 3,
	RENDER_TARGET_LIGHTING_GROUP_MEMBER_COUNT = 2,
	RENDER_TARGET_GROUP_COUNT = static_cast<uint8>(RENDER_TARGET_GROUP_TYPE::END)
};


struct RenderTarget
{
	shared_ptr < class Texture > target;
	float clearColor[4];
};

class RenderTargetGroup
{
public:
	void Create(RENDER_TARGET_GROUP_TYPE groupType, vector<RenderTarget>& rtVec, shared_ptr<Texture> dsTexture);

	void OMSetRenderTargets(uint32 count, uint32 offset);
	void OMSetRenderTargets();

	void ClearRenderTargetView(uint32 index);
	void ClearRenderTargetView();

	shared_ptr<Texture> GetRTTexture(uint32 index) { return _rtVec[index].target; }
	shared_ptr<Texture>  GetDSTexture() { return _depthStencilTexture; }


private:
	RENDER_TARGET_GROUP_TYPE		_groupType;
	vector<RenderTarget>			_rtVec;
	uint32							_rtCount;
	// RTV
	vector<ComPtr<ID3D11RenderTargetView>> _renderTargetView;
	// DSV
	shared_ptr<Texture> _depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> _depthStencilView;
};


