#include "stdafx.h"
#include <timeapi.h>
#include "CDx11Render.h"
#include "WICTextureLoader.h"

//������������
//Ӳ���豸��hardware device�����ο��豸��reference device������������豸��software driver device���� �Լ�WARP�豸 ��WARP device����
//Ӳ���豸��hardware device����һ���������Կ��ϵ�D3D�豸���������豸�������ٶ������ġ��⽫�������պ���������һ�����͡�
//�ο��豸��reference device��������û�п��õ�Ӳ��֧��ʱ��CPU�Ͻ�����Ⱦ���豸��
//							����֮���ο��豸���������������CPU��Ӳ����Ⱦ�豸��һ��ģ�⡣���ǲ��ҵ��ǣ����ַ�ʽ�ǳ��ĵ�Ч�������ڿ��������У�
//							û����������ѡ���ʱ�����ǲŲ������ַ�ʽ��������һ����DirectX�����ˣ������ϻ�û��֧�������°汾DirectX��Ӳ����
//							�����ڿ��������о�ֻ�ܲ������ַ�ʽ�����ˡ�
//��������豸��software driverdevice���ǿ�����Ա�Լ���д������Direct3D����Ⱦ������������ַ�ʽͨ�����Ƽ����ڸ����ܻ��߶�����Ҫ����̵�Ӧ�ó���������ܵ�WARP�豸���Ǹ��õ�ѡ��
//WARP�豸��WARPdevice����һ�ָ�Ч��CPU��Ⱦ�豸������ģ���ֽ׶����е�Direct3D���ԡ�WARPʹ����Windows Vista / Windows 7 / Winodws 8�е�Windows Graphic ���п��и߶��Ż����Ĵ�����Ϊ֧�ţ�
//							�������ַ�ʽ������ͣ�����������ᵽ�Ĳο��豸��reference device��ģʽ�������㡣WARP�豸�����ò��ߵĻ���������Դﵽ������Ϊ����Ĺ�Ч�������ǵ�Ӳ����֧��ʵʱӦ�ó���real - time application��������£�
//							��WARP�豸��Ϊ�油��һ�����ǵ�ѡ����Ϊ��ȶ��ԣ��ο��豸��reference device����ִ��Ч��ʵ�����޷����˹�ά��������ˣ�WARP�豸��ִ��Ч�ʻ��ǲ��ܺ�Ӳ���豸ͬ�ն���Ͼ��������Ƕ�Ӳ����һ��ģ�⣬��ʹ����ģ���Ƿǳ���Ч�ġ�
D3D_DRIVER_TYPE g_DriverTypes[] =
{
	D3D_DRIVER_TYPE_HARDWARE,
	D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE
};
UINT g_NumDriverTypes = ARRAYSIZE(g_DriverTypes);

//������������
//Direct3D�������ȼ�����ָ����Ҫ�趨���豸Ŀ�ꡣ�����ר��֮�У����ǽ���������豸����һ�ֵ�Ȼ�����ǵ�Direct3D 11�豸���ڶ���ΪDirect3D 10.1�豸��������ΪDirect3D 10.0�豸�����������豸���޷�֧�ֵ�����£�������ѡ��WARP�豸���߲ο��豸��Ϊ��Ԯ��
D3D_FEATURE_LEVEL g_FeatureLevels[] =
{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0
};
UINT g_NumFeatureLevels = ARRAYSIZE(g_FeatureLevels);

CDx11Render::CDx11Render()
	:m_hTargetWnd(NULL)
{
}


CDx11Render::~CDx11Render()
{
}

bool CDx11Render::init(HWND hTargetWnd, int nWidth, int nHeight)
{
	if (nullptr != m_device)
	{
		return false;
	}

	if (!::IsWindow(hTargetWnd))
	{
		return false;
	}
	m_hTargetWnd = hTargetWnd;
	HRESULT hRet = S_OK;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//������
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));//���
	scd.BufferCount = 1;                          //����ֻ����һ���󻺳壨˫���壩���Ϊ1
	scd.BufferDesc.Width = nWidth;
	scd.BufferDesc.Height = nHeight;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_hTargetWnd;
	scd.SampleDesc.Count = 1;                      //1�ز���
	scd.SampleDesc.Quality = 0;                      //�����ȼ�
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;      //���ò���
	scd.Windowed = TRUE;                            //�Ƿ�ȫ��
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;	//�����ȼ�  

	/*��������Direct3D��Ϊһ���豸��ȾĿ��ļ��ϡ�ÿһ���豸��������һ����������������������ܹ�������豸��������һ������Ŀ�����Ϊһ����Ⱦ����ʾ����Ļ�ϵ���ɫ���棨�ں�������ۣ����ȵȡ�
	ͨ������Ϸ���У���������ɫ���棬�ֱ����������͸������棬����һ�𱻳�Ϊǰ��̨������ϡ��������е����ݣ�ǰ̨���棩����ʾ����Ļ�ϣ����������棨��̨���棩���ڻ�����һ֡��
	��Ⱦ�ķ����ǳ�֮�죬��Ļ��һ���ֿ�������ʾ�������ʾ����֮ǰ������ǰ�Ľ��Ϊ�����Ͻ��л��ơ�����֮����л������Խ���һ�����Ե�������ǰ̨����ʾͼ�񣬺�̨����Ϊǰ̨׼����һ�̽�Ҫ��ʾ��ͼ�����������Ա���ܶ༬�ֵ����⣬�����Ч�ʡ�
	���ּ����ڼ����ͼ��ѧ�н���˫���壨doublebuffering�������߽�ҳ�淭ת��page flipping����һ����������ӵ��һ�����߶�������Ļ��塣
	*/
	for (UINT driverTypeIndex = 0; driverTypeIndex < g_NumDriverTypes; ++driverTypeIndex)
	{
		D3D_DRIVER_TYPE nDriverType = g_DriverTypes[driverTypeIndex];
		hRet = D3D11CreateDeviceAndSwapChain(
			NULL,                               //Ĭ��ͼ��������
			nDriverType,                        //��������
			NULL,                               //ʵ�������Ⱦ�豸�Ķ�̬���������ʹ�õ������豸����������豸����ΪNULL
			createDeviceFlags,                  //������־��0������Ϸ������һ��D3D11_CREATE_DEVICE_DEBUG�������Ǵ����ɹ����Ե��豸���ڿ����бȽ�����
			g_FeatureLevels,                    //�����ȼ�
			g_NumFeatureLevels,                 //�����ȼ�����
			D3D11_SDK_VERSION,                  //sdk�汾��
			&scd,
			&m_swapChain,
			&m_device,
			&featureLevel,
			&m_deviceContext
		);
		if (SUCCEEDED(hRet))
			break;
	}
	if (FAILED(hRet))
		return false;

	//��ȡ�󻺳�����ַ
	hRet = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_depthStencilBuffer);
	if (FAILED(hRet))
		return false;

	//����Ŀ����ͼ
	hRet = m_device->CreateRenderTargetView(m_depthStencilBuffer.Get(), NULL, &m_renderTargetView);
	//�ͷź󻺳�
	m_depthStencilBuffer = nullptr;
	if (FAILED(hRet))
		return false;

	ID3D11RenderTargetView* rtv[1] = { m_renderTargetView.Get() };
	//�󶨵���Ⱦ����
	m_deviceContext->OMSetRenderTargets(1, rtv, NULL);

	//����viewport
	D3D11_VIEWPORT vp;
	vp.Height = (FLOAT)nWidth;
	vp.Width = (FLOAT)nHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_deviceContext->RSSetViewports(1, &vp);//������ȾĿ����ͼ

	_intFont();

	//CreateWICTextureFromFileEx(m_device, L"F:\\test.jpg", {3840,2160}, )

	m_image.init(m_device, m_deviceContext, 1280, 720);

	::timeBeginPeriod(1);
	return true;
}

void CDx11Render::render(void)
{
	if (!IsWindow(m_hTargetWnd))
	{
		return;
	}
	static int nLastCount = 0;
	static DWORD dwLastTime = timeGetTime();
	static int nLastFps = 0;
	static wstring strText = L"";
	++nLastCount;
	float ClearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f }; //red,green,blue,alpha
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), ClearColor);
	drawString(strText, 100.0f, 100.0f);
	//m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	m_image.render(m_deviceContext);

	m_swapChain->Present(0, 0);

	DWORD dwTime = timeGetTime() - dwLastTime;
	if (dwTime >= 2000)
	{
		nLastFps = nLastCount * 1000 / 2000;
		nLastCount = 0;
		dwLastTime = timeGetTime();
		wchar_t wcBuffer[256] = { 0 };
		wsprintf(wcBuffer, L"%dFps", nLastFps);
		strText = wcBuffer;

		OutputDebugStringW(wcBuffer);
		OutputDebugStringW(L"\n");
	}
}

void CDx11Render::uninit(void)
{
	::timeEndPeriod(1);
	if (m_deviceContext)
	{
		m_deviceContext->ClearState();
	}

	m_swapChain = nullptr;
	m_renderTargetView = nullptr;
	m_deviceContext = nullptr;
	m_device = nullptr;
	_unintFont();
}

bool CDx11Render::drawString(const wstring& strText, float startX, float startY)
{
	m_fontPos.x = startX;
	m_fontPos.y = startY;
	m_spriteBatch->Begin();

	Vector2 origin = m_font->MeasureString(strText.c_str()) / 2.f;

	m_font->DrawString(m_spriteBatch.get(), strText.c_str(),
		m_fontPos, Colors::White, 0.f, origin);

	m_spriteBatch->End();
	return true;
}

void CDx11Render::_intFont(void)
{
	m_font = std::make_unique<SpriteFont>(m_device.Get(), L"myfileb.spritefont");
	m_spriteBatch = std::make_unique<SpriteBatch>(m_deviceContext.Get());
}

void CDx11Render::_unintFont(void)
{
	m_font.reset();
	m_spriteBatch.reset();
}