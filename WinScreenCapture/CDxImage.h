#pragma once

#include <d3d11.h>
#include <WRL/client.h>

using namespace Microsoft::WRL;

class CDxImage
{
public:
	CDxImage();
	~CDxImage();

	bool init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext, int nWidth, int nHeight);

	bool updateYUV(ComPtr<ID3D11DeviceContext> deviceContext, const unsigned char *yuv[3], int linesize[3]);

	bool render(ComPtr<ID3D11DeviceContext> deviceContext);

private:
	ID3D11ShaderResourceView* m_resViewList[3];
	ComPtr<ID3D11Texture2D> m_textureList[3];
	ComPtr<ID3D11VertexShader> m_pVertexShader;	// 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;	// 像素着色器

	int m_nWidth;
	int m_nHeight;
};

