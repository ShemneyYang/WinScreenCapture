#include "stdafx.h"
#include <timeapi.h>
#include "CDx11Render.h"
#include "WICTextureLoader.h"

//驱动类型数组
//硬件设备（hardware device），参考设备（reference device），软件驱动设备（software driver device）， 以及WARP设备 （WARP device）。
//硬件设备（hardware device）是一个运行在显卡上的D3D设备，在所有设备中运行速度是最快的。这将是我们日后讨论最多的一种类型。
//参考设备（reference device）是用于没有可用的硬件支持时在CPU上进行渲染的设备。
//							简言之，参考设备就是利用软件，在CPU对硬件渲染设备的一个模拟。但是不幸的是，这种方式非常的低效，所以在开发过程中，
//							没有其他可用选择的时候，我们才采用这种方式。比如新一代的DirectX发布了，市面上还没有支持这种新版本DirectX的硬件，
//							我们在开发过程中就只能采用这种方式来跑了。
//软件驱动设备（software driverdevice）是开发人员自己编写的用于Direct3D的渲染驱动软件。这种方式通常不推荐用于高性能或者对性能要求苛刻的应用程序，下面介绍的WARP设备将是更好的选择。
//WARP设备（WARPdevice）是一种高效的CPU渲染设备，可以模拟现阶段所有的Direct3D特性。WARP使用了Windows Vista / Windows 7 / Winodws 8中的Windows Graphic 运行库中高度优化过的代码作为支撑，
//							这让这种方式出类拔萃，相比与上文提到的参考设备（reference device）模式更加优秀。WARP设备在配置不高的机器上面可以达到化腐朽为神奇的功效。在我们的硬件不支持实时应用程序（real - time application）的情况下，
//							用WARP设备作为替补是一个明智的选择，因为相比而言，参考设备（reference device）的执行效率实在是无法令人恭维。即便如此，WARP设备的执行效率还是不能和硬件设备同日而语，毕竟它依旧是对硬件的一种模拟，即使这种模拟是非常高效的。
D3D_DRIVER_TYPE g_DriverTypes[] =
{
	D3D_DRIVER_TYPE_HARDWARE,
	D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE
};
UINT g_NumDriverTypes = ARRAYSIZE(g_DriverTypes);

//特征级别数组
//Direct3D的特征等级用于指定需要设定的设备目标。在这个专栏之中，我们将针对三种设备，第一种当然是我们的Direct3D 11设备，第二种为Direct3D 10.1设备，第三种为Direct3D 10.0设备。再这三种设备都无法支持的情况下，我们再选择WARP设备或者参考设备作为后援。
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

	//交换链
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));//填充
	scd.BufferCount = 1;                          //我们只创建一个后缓冲（双缓冲）因此为1
	scd.BufferDesc.Width = nWidth;
	scd.BufferDesc.Height = nHeight;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = m_hTargetWnd;
	scd.SampleDesc.Count = 1;                      //1重采样
	scd.SampleDesc.Quality = 0;                      //采样等级
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;      //常用参数
	scd.Windowed = TRUE;                            //是否全屏
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;	//特征等级  

	/*交换链在Direct3D中为一个设备渲染目标的集合。每一个设备都有至少一个交换链，而多个交换链能够被多个设备所创建。一个交换目标可以为一个渲染和显示到屏幕上的颜色缓存（在后面会讨论），等等。
	通常在游戏中有，有两种颜色缓存，分别叫做主缓存和辅助缓存，他们一起被称为前后台缓存组合。主缓存中的内容（前台缓存）会显示在屏幕上，而辅助缓存（后台缓存）用于绘制下一帧。
	渲染的发生非常之快，屏幕的一部分可以在显示器完成显示更新之前，在先前的结果为基础上进行绘制。缓存之间的切换，可以进行一个良性的运作，前台在显示图像，后台正在为前台准备下一刻将要显示的图像，这样做可以避免很多棘手的问题，提高了效率。
	这种技术在计算机图形学中叫做双缓冲（doublebuffering），或者叫页面翻转（page flipping）。一个交换链能拥有一个或者多个这样的缓冲。
	*/
	for (UINT driverTypeIndex = 0; driverTypeIndex < g_NumDriverTypes; ++driverTypeIndex)
	{
		D3D_DRIVER_TYPE nDriverType = g_DriverTypes[driverTypeIndex];
		hRet = D3D11CreateDeviceAndSwapChain(
			NULL,                               //默认图形适配器
			nDriverType,                        //驱动类型
			NULL,                               //实现软件渲染设备的动态库句柄，如果使用的驱动设备类型是软件设备则不能为NULL
			createDeviceFlags,                  //创建标志，0用于游戏发布，一般D3D11_CREATE_DEVICE_DEBUG允许我们创建可供调试的设备，在开发中比较有用
			g_FeatureLevels,                    //特征等级
			g_NumFeatureLevels,                 //特征等级数量
			D3D11_SDK_VERSION,                  //sdk版本号
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

	//获取后缓冲区地址
	hRet = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&m_depthStencilBuffer);
	if (FAILED(hRet))
		return false;

	//创建目标视图
	hRet = m_device->CreateRenderTargetView(m_depthStencilBuffer.Get(), NULL, &m_renderTargetView);
	//释放后缓冲
	m_depthStencilBuffer = nullptr;
	if (FAILED(hRet))
		return false;

	ID3D11RenderTargetView* rtv[1] = { m_renderTargetView.Get() };
	//绑定到渲染管线
	m_deviceContext->OMSetRenderTargets(1, rtv, NULL);

	//设置viewport
	D3D11_VIEWPORT vp;
	vp.Height = (FLOAT)nWidth;
	vp.Width = (FLOAT)nHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_deviceContext->RSSetViewports(1, &vp);//创建渲染目标视图

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