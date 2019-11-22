#pragma once
#include <memory>
#include <d3d11.h>
#include <WRL/client.h>
#include "SpriteFont.h"
#include "SimpleMath.h"
#include "CDxImage.h"

using namespace Microsoft::WRL;
using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class CDx11Render
{
public:
	CDx11Render();
	virtual ~CDx11Render();

	bool init(HWND hTargetWnd, int nWidth, int nHeight);
	void uninit(void);
	void render(void);
	bool drawString(const wstring& strText, float startX, float startY);

private:
	void _intFont(void);
	void _unintFont(void);

private:
	HWND m_hTargetWnd;

	ComPtr<IDXGISwapChain> m_swapChain;
	ComPtr<ID3D11Device> m_device;
	ComPtr<ID3D11DeviceContext> m_deviceContext;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	ComPtr<ID3D11Texture2D> m_depthStencilBuffer;

	Vector2 m_fontPos;
	unique_ptr<SpriteBatch> m_spriteBatch;
	unique_ptr<SpriteFont> m_font;

	CDxImage m_image;
};

