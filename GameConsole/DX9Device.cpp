#include "DX9Device.h"

using namespace calyx;

DX9Device::DX9Device(HWND hDeviceWindow)
	: m_hDeviceWindow(hDeviceWindow)
{
}

DX9Device::~DX9Device()
{
}

int DX9Device::CreateDeivce()
{
	HRESULT hr;
	BOOL windowed = TRUE;

	// 1. Get interface to Direct3D
	m_pd3dObject = Direct3DCreate9(D3D_SDK_VERSION);
	if (!m_pd3dObject)
	{
		MessageBox(NULL, TEXT("Failed to create Direct3D object"), NULL, NULL);
		return false;
	}

	// 2. Get video card information
	// Or we can just use D3DADAPTER_DEFAULT, which always works.

	// 	UINT videoCardCount = m_pD3dObject->GetAdapterCount();
	// 	D3DADAPTER_IDENTIFIER9* videoCards = (D3DADAPTER_IDENTIFIER9*)malloc(sizeof(*videoCards)*videoCardCount);
	// 	for (UINT i = 0; i < videoCardCount; i++)
	// 	{
	// 		printf("GetAdapterIdentifier()\n");
	// 		m_pD3dObject->GetAdapterIdentifier(i, 0, &videoCards[i]);
	// 	}
	// 
	// 	// ��ӡ��Ƶ����Ϣ
	// 	printf("Adapter List:\n");
	// 	for (UINT i = 0; i < videoCardCount; i++)
	// 	{
	// 		printf("%s\n", videoCards[i].DeviceName);
	// 	}

	// 3. ����豸����
	D3DDEVTYPE devicetype = D3DDEVTYPE_HAL;
	// 	struct DeviceTypeFormat
	// 	{
	// 		D3DDEVTYPE	type;
	// 		D3DFORMAT	format;
	// 	} dtfs[] = {
	// 		{D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8},
	// 		{D3DDEVTYPE_HAL, D3DFMT_X1R5G5B5},
	// 		{D3DDEVTYPE_HAL, D3DFMT_R5G6B5},
	// 		{D3DDEVTYPE_REF, D3DFMT_X8R8G8B8},
	// 		{D3DDEVTYPE_REF, D3DFMT_X1R5G5B5},
	// 		{D3DDEVTYPE_REF, D3DFMT_R5G6B5}
	// 	};
	// 	D3DFORMAT format;
	// 	D3DDEVTYPE devicetype;
	// 	
	// 	int i;
	// 	for (i = 0; i < arraysize(dtfs); ++i)
	// 	{
	// 		// ����ȫ������DisplayFormat��BackBufferForamt������ͬ
	// 		hr = m_pd3dObject->CheckDeviceType(D3DADAPTER_DEFAULT, dtfs[i].type, dtfs[i].format, dtfs[i].format, windowed);
	// 		if (hr == D3D_OK)
	// 			break;
	// 	}
	//format = dtfs[i].format;

	// 4. Check device capabilities
	D3DCAPS9 caps;
	hr = m_pd3dObject->GetDeviceCaps(D3DADAPTER_DEFAULT, devicetype, &caps);
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("Failed to get Direct3D device caps"), NULL, NULL);
		return false;
	}

	// ���Ӳ���任�͹���
	DWORD vtx_proc;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		vtx_proc = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE) {
			// �������������ٶȣ��������ڵ���
			vtx_proc |= D3DCREATE_PUREDEVICE;
		}
	} else {
		vtx_proc = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// Check present interval
	// 	DWORD presinterval = 0;
	// 	if (caps.PresentationIntervals & D3DPRESENT_INTERVAL_FOUR)
	// 	{
	// 		presinterval = D3DPRESENT_INTERVAL_FOUR;
	// 	}

	// 5. Fill out the present parameters

	// 5.1 ��ȡ��ǰ����ʾģʽ
	D3DDISPLAYMODE d3ddm;
	if (FAILED(m_pd3dObject->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm)))
		return false;

	ZeroMemory(&m_d3dPresent, sizeof(m_d3dPresent));
	m_d3dPresent.Windowed = windowed;						// ����ģʽ
	//m_d3dPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// ��������, Ĭ��ֵΪ��ʹ�ý���֡�ʵ�����Ļˢ����
	m_d3dPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;		//��ģʽ���Դ����κ�MultiSampleType(D3DMULTISAMPLE_NONE����ֻ��ʹ�ø�Ч��)
	m_d3dPresent.BackBufferFormat = d3ddm.Format;			// �ڴ���ģʽ�£�Ĭ��ֵΪ(0)ʹ�õ�ǰ��ʾģʽ
	m_d3dPresent.BackBufferCount = 1;						// �󱸻���������
	m_d3dPresent.EnableAutoDepthStencil = TRUE;
	m_d3dPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dPresent.MultiSampleType = D3DMULTISAMPLE_NONE;		// ���SwapEffect����D3DSWAPEFFECT_DISCARDʱ��������D3DMULTISAMPLE_NONE
	m_d3dPresent.hDeviceWindow = m_hDeviceWindow;				// ����ģʽ�£�Ĭ��(NULL)Ϊ�����������豸ʱָ����focus window

	// Create the D3dDevice
	hr = m_pd3dObject->CreateDevice(D3DADAPTER_DEFAULT, devicetype, m_hDeviceWindow, vtx_proc, &m_d3dPresent, &m_pDevice);
	if (FAILED(hr))
	{
		MessageBox(NULL, TEXT("Failed to create Direct3D device"), NULL, NULL);
		return false;
	}

	// Check vertex index format
	m_pDevice->GetDeviceCaps(&caps);
	if (caps.MaxVertexIndex > 0x0000FFFF)
		m_d3dVertexIndexFormat = D3DFMT_INDEX32;

#if 0
	// �����ӿ�(�Ǳ���)
	D3DVIEWPORT9 vp = { 20, 20, 640, 480, 0, 1 };
	m_pDevice->SetViewport(&vp);
#endif

	return true;
	return 0;
}

int DX9Device::TryResetDevice()
{
	return 0;
}

int DX9Device::DestroyDevice()
{
	return 0;
}