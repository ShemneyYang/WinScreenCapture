#include "stdafx.h"
#include "CDxImage.h"
#include "d3dUtil.h"


bool GetFileSize(const std::wstring& strFilePath, ULONGLONG& nFileSize)
{
	HANDLE hFileHandle = CreateFile(strFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFileHandle)
	{
		return false;
	}

	LARGE_INTEGER filesize;

	if (GetFileSizeEx(hFileHandle, &filesize))
	{
		nFileSize = filesize.QuadPart;
		::CloseHandle(hFileHandle);
		return true;
	}
	::CloseHandle(hFileHandle);
	return false;
}


bool ReadFileData(const std::wstring &fileName, std::string &data)
{
	ULONGLONG fileSize = 0;
	if (GetFileSize(fileName, fileSize) == false)
	{
		return false;
	}

	//the file size must be less than 16 MB
	//DW_ASSERT_X(fileSize <= 16 * 1024 * 1024, __FUNCTION__, "Can't read more 16MB file data in one time.");

	FILE *fp = NULL;
	_wfopen_s(&fp, fileName.c_str(), L"rb");
	if (fp)
	{
		data.resize((size_t)fileSize);
		size_t count = fread((void*)data.data(), 1, data.size(), fp);
		fclose(fp);

		if (count == data.size())
		{
			return true;
		}
	}

	data.clear();
	return false;
}

CDxImage::CDxImage():
	m_nWidth(0),
	m_nHeight(0)
{
}


CDxImage::~CDxImage()
{
}


bool CDxImage::init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext, int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8_UNORM, m_nWidth, m_nHeight);
	textureDesc.MipLevels = 1;

	HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &m_textureList[0]);

	textureDesc.Width = m_nWidth / 2; 
	textureDesc.Height = m_nHeight / 2;
	hr = device->CreateTexture2D(&textureDesc, NULL, &m_textureList[1]);
	hr = device->CreateTexture2D(&textureDesc, NULL, &m_textureList[2]);

	CD3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D);
	for (int i = 0; i < 3; ++i)
	{
		device->CreateShaderResourceView(m_textureList[i].Get(), &resourceViewDesc, &m_resViewList[i]);
	}

	ComPtr<ID3DBlob> blob;
	// 创建顶点着色器
	hr = CreateShaderFromFile(L"HLSL\\vs.cso", L"HLSL\\vs.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf());
	hr = device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf());

	// 创建像素着色器
	hr = CreateShaderFromFile(L"HLSL\\ps.cso", L"HLSL\\ps.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf());
	hr = device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf());

	std::string strYdata;
	ReadFileData(L"res\\y.dat", strYdata);
	std::string strUdata;
	ReadFileData(L"res\\u.dat", strUdata);
	std::string strVdata;
	ReadFileData(L"res\\v.dat", strVdata);

	const unsigned char *yuv[3] = { (const unsigned char *)strYdata.c_str(), (const unsigned char *)strUdata.c_str(), (const unsigned char *)strVdata.c_str() };
	int linesize[3] = { strYdata.size(), strUdata.size(), strVdata.size() };
	updateYUV(deviceContext, yuv, linesize);
	return false;
}

bool CDxImage::updateYUV(ComPtr<ID3D11DeviceContext> deviceContext, const unsigned char *yuv[3], int linesize[3])
{
	deviceContext->UpdateSubresource(m_textureList[0].Get(), 0, NULL, yuv[0], linesize[0], 0);
	deviceContext->UpdateSubresource(m_textureList[1].Get(), 0, NULL, yuv[1], linesize[1], 0);
	deviceContext->UpdateSubresource(m_textureList[2].Get(), 0, NULL, yuv[2], linesize[2], 0);
	return true;
}

bool CDxImage::render(ComPtr<ID3D11DeviceContext> deviceContext)
{
	// 将着色器绑定到渲染管线
	deviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	deviceContext->PSSetShaderResources(0, 3, m_resViewList);
	return true;
}