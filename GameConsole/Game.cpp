#include <stdio.h>
#include <assert.h>
//#include <WinInet.h>	// URL
#include "Game.h"
#include "Calyx.h"
#include "modules.h"
#include "MyModels.h"

namespace
{
    calyx::D3D9Console* g_pApp;

    // non-member window procedure
    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return g_pApp->WindowProc(hWnd, msg, wParam, lParam);
    }
}

using namespace calyx;

D3D9Console::D3D9Console()
{

}

D3D9Console::~D3D9Console()
{

}


#if 0
static int calyx_lua_size(lua_State *L)
{
    int n = lua_gettop(L);
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));

    if (n == 2) 
    {
        int cx = (int)luaL_optnumber(L, 1, 0.0);
        int cy = (int)luaL_optnumber(L, 2, 0.0);
        // ��������
        self->CreateD3D9Window(cx, cy);
    }
    // TODO ������

    return 0;
}
#endif


// �¼��������
// TODO �¼�����
static int calyx_lua_event_post(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    if (n >= args) {
        std::string event = luaL_optstring(L, args++, "");
        D3D9Console* self = (D3D9Console*)lua_touserdata(L, lua_upvalueindex(1));
        assert(self != NULL);
        if ("pause" == event) {
            self->Pause(true);
        } else if ("run" == event) {
            self->Pause(false);
        } else if ("quit" == event) {
            self->Quit();
        }
    }
    return 0;
}


int D3D9Console::InitLua()
{
    L = luaL_newstate();
    luaL_openlibs(L);

    // Ԥ���غ���ģ��
    // ��װ�ڽ�ģ��Ԥ�������
    init_modules(L);

    // ָʾ���浱ǰ�ڶ���ģʽ�����У�.exeģʽ��
    //{
    //    lua_pushboolean(L, 1);
    //    lua_setfield(L, -2, "standalone");
    //}

    // ���������ű�
    // TODO ��Ϊ��ģ��Ԥ���룬Ȼ���ڴ˴�require
    luaL_dofile(L, "boot.lua");

    // ��ȡ���ڱ���ʹ�С��Ϣ
    // TODO ��LuaRuntimeȥ�������Ƕ�׵ı�
    lua_getglobal(L, "calyx");
    lua_getfield(L, -1, "conf");
    lua_getfield(L, -1, "app");
    lua_getfield(L, -1, "title");
    const char* ansiTitle = lua_tostring(L, -1);

#if defined(UNICODE) || defined(_UNICODE)
    wchar_t* unicodeTitle = ansi_to_unicode(ansiTitle);
    m_sAppTitle.assign(unicodeTitle);
    free(unicodeTitle);
#else
    m_sAppTitle.assign(ansiTitle);
#endif // defined(UNICODE) || defined(_UNICODE)

    // �ضϹ����ı���
    m_sAppTitle = m_sAppTitle.substr(0, 128);

    // ��thisָ�뱣�浽Luaע���
    lua_pushlightuserdata(L, this);
    lua_setfield(L, LUA_REGISTRYINDEX, "thisapp");

    // ����printf����
    luaopen_printf(L);

    // �¼�����ϵͳ
    // event.post = c_event_post
    lua_getglobal(L, "calyx");
    lua_getfield(L, -1, "event");
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_event_post, 1);
    lua_setfield(L, -2, "post");    

    // ͼ�η���ĺ���-------------------
    // TODO �������ѭ��

    lua_newtable(L);
    lua_setglobal(L, "screen");

    // ��Ļ�ֱ���
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    lua_getglobal(L, "screen");
    lua_pushinteger(L, screen_width);
    lua_setfield(L, -2, "width");

    lua_getglobal(L, "screen");
    lua_pushinteger(L, screen_height);
    lua_setfield(L, -2, "height");

    // ���ô��ڴ�С
    //lua_pushlightuserdata(L, this);
    //lua_pushcclosure(L, calyx_lua_size, 1);
    //lua_setglobal(L, "size");

    return 1;
}

int D3D9Console::Run()
{
    // ������Ϸ�ű�
	if (luaL_dofile(L, "main.lua"))
    {
        TCHAR message[1024];
        wsprintf(message, TEXT("%s\n"), ansi_to_unicode(lua_tostring(L, -1)));
        MessageBox(NULL, message, TEXT("Calyx"), MB_OK|MB_ICONSTOP);
        return false;
    }

    // ������ں���
    lua_getglobal(L, "setup");
    if (lua_pcall(L, 0, 0, 0))
        return false;

    // ����ϵͳ��Ϣ
    m_pTimer->Start();
    MSG msg = {0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&msg);	// �����������ΪASCII
            DispatchMessage(&msg);
        }
        else
        {
            //// ����Lua�¼�
            //if (!m_msgQueue.empty()) {
            //	int event = m_msgQueue.pop();
            //	DispatchEvent(event);
            //}

            float deltaTime = m_pTimer->Elapsed();
            if (!m_bSuspended)
            {
                Update(deltaTime);
            }
            Draw();
            CalculateFPS(deltaTime);
        }
    }
    Shutdown();
    return (int)(msg.wParam);
}

void D3D9Console::Update(float dt)
{
    lua_getglobal(L, "update");
    lua_pushnumber(L, dt);
    lua_pcall(L, 1, 0, 0);
}


// ��ͼ����
void D3D9Console::Draw()
{
    // �����豸��ʧ
    if (m_bHandleDeviceLost)
    {
        TryResetDevice();
    }

    // ����
    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_bgcolor, 1.0f, 0);

    // ���û�ͼ����
    m_pDevice->BeginScene();

    // ���ýű���ͼ����
    lua_getglobal(L, "draw");
    lua_pcall(L, 0, 0, 0);

    // ����������
    m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 12, cube_vertices, sizeof(D3DXVECTOR4));

    // ������������
    m_pDevice->EndScene();

    // ����豸�Ƿ�ʧ
    if (D3DERR_DEVICELOST == m_pDevice->Present(0, 0, 0, 0))
    {
        m_bDeviceLost = true;
    }
}

bool D3D9Console::AcquireDeviceResources()
{
    m_pDevice->CreateVertexBuffer(24 * sizeof(d3d::VertexNormTex), 0, 
        d3d::VertexNormTex::FVF, D3DPOOL_MANAGED, &m_pVB, NULL);

    m_pDevice->CreateIndexBuffer(36 * sizeof(DWORD),
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL);

    return true;
}

void D3D9Console::ReleaseDeviceResources()
{
    SafeRelease(&m_pVB);
    SafeRelease(&m_pIB);
    SafeRelease(&m_pSprite);
}

//bool D3D9Console::OnLostDevice()
//{
//    m_pSprite->OnLostDevice();
//    return true;
//}
//
//// �����豸��ʧ��Ļָ�
//bool D3D9Console::OnResetDevice()
//{
//    m_pSprite->OnResetDevice();
//    ResetGraphicsState();
//    return true;
//}

// ����ͼ���豸״̬
bool D3D9Console::ResetGraphicsState()
{
    // ������������ͼ����
    D3DXVECTOR3 eye (1.5f, 0.75f, -1); //������iso
    //D3DXVECTOR3 eye (1.5f, 0, -1);
    D3DXVECTOR3 target (0, 0, 0);
    D3DXVECTOR3 up (0, 1, 0);
    D3DXMATRIXA16 view;

    D3DXMatrixLookAtLH(&view, &eye, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &view);

    // ����������ͶӰ����
    RECT rect;
    GetClientRect(m_hAppWindow, &rect);
    float aspect = (float)(rect.right - rect.left) / (rect.bottom - rect.top);
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, aspect, 1.0f, 1000.0f);
    //D3DXMatrixOrthoLH(&proj, 5.0f, 5.0f * aspect, 0.0f, 5.0f);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, 1);	// ��������
    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);	// ���Բ�ֵ��ɫ
    m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, 1);	// ������λ����������
    m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, 1); // ���������

    return true;
}

// ��ʼ��ͼ����ϵͳ
bool D3D9Console::InitGraphics()
{
    // �������ж�ȡ���ڿͻ�����С
    // TODO  ���ǰ���������������
    lua_getglobal(L, "calyx");
    lua_getfield(L, -1, "conf");
    lua_getfield(L, -1, "app");
    lua_getfield(L, -1, "width");
    int cx = lua_tointeger(L, -1);

    lua_getglobal(L, "calyx");
    lua_getfield(L, -1, "conf");
    lua_getfield(L, -1, "app");
    lua_getfield(L, -1, "height");
    int cy = lua_tointeger(L, -1);

    // ��������
    CreateApplicationWindow(cx, cy);

    // ��ʾ����
    ShowWindow(m_hAppWindow, SW_SHOW);

    // ��ʼ��D3D
    InitDirect3D();

    // ����ͼ���豸��Դ
    if (!AcquireDeviceResources())
        return false;

    // ����ͼ���豸״̬
    ResetGraphicsState();

    // ��������
    D3DXCreateSprite(m_pDevice, &m_pSprite);

    // ����ȱʡ����
    if (FAILED(D3DXCreateFont(m_pDevice, 20, 0, 0, 1, 0,
        DEFAULT_CHARSET,
        OUT_OUTLINE_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        TEXT("fixedsys"),
        &m_font)))  {
            return false;
    }

    return true;
}

void D3D9Console::Pause(bool status)
{
    m_bSuspended = status;
}

void D3D9Console::Quit()
{
    ::PostQuitMessage(0);
}

int D3D9Console::Init(HINSTANCE hInstance)
{
    m_bWireFrame = false;
    m_bLight = true;
    m_bgcolor = d3d::Color::Cornflower; // ȱʡ����ɫ
    m_hInstance = hInstance;
    m_hAppWindow = NULL;
    m_pd3dObject = NULL;
    m_pDevice = NULL;
    m_d3dVertexIndexFormat = D3DFMT_INDEX16;
    m_bSuspended = false;
    m_bDeviceLost = false;
    m_bHandleDeviceLost = true;
    g_pApp = this;
    m_fps = 0;
    m_pTimer = new GameTime;

    if (!m_pTimer->Init())
        return false;

    // ��ʼ��Lua�������ű�
    if (!InitLua())
        return false;

    if (!InitGraphics())
        return false;

    //InitInput();
    //InitGUI();

    return true;
}

bool D3D9Console::CreateApplicationWindow(int cx, int cy)
{
    static const TCHAR window_class_name[] = TEXT("D3D9Game Window CLass");

    WNDCLASSEX wcex;
    ZeroMemory(&wcex, sizeof wcex);
    wcex.cbSize			= sizeof wcex;
    wcex.cbClsExtra		= 0;									// No Extra Window Data
    wcex.cbWndExtra		= 0;									// No Extra Window Data
    wcex.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
    wcex.hInstance		= m_hInstance;							// Set The Instance
    wcex.lpfnWndProc	= WndProc;								// WndProc Handles Messages
    wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);		// Load The Default Icon
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
    wcex.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);	// No Background Required For GL
    wcex.lpszMenuName	= NULL;									// We Don't Want A Menu
    wcex.lpszClassName	= window_class_name;					// Set The Class Name
    wcex.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))								// Attempt To Register The Window Class
    {
        MessageBox(NULL, TEXT("Failed To Register The Window Class"), NULL, NULL);
        return false;
    }

    // ȱʡ�Ĵ���λ��
    static int _initX = 200;			
    static int _initY = 100;
    // Create Window
    DWORD		dwExStyle;				// Window Extended Style
    DWORD		dwStyle;				// Window Style
    RECT WindowRect = {_initX, _initY, _initX + cx, _initY + cy};
    dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
    dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    TCHAR title_buf[256];
    wsprintf(title_buf, TEXT("%s %dx%d"), m_sAppTitle.c_str(), cx, cy);

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

    m_hAppWindow = CreateWindowEx(dwExStyle,		// Extended Style For The Window
        window_class_name,							// Class Name
        title_buf,									// Window Title
        dwStyle |									// Defined Window Style
        WS_CLIPSIBLINGS |							// Required Window Style
        WS_CLIPCHILDREN,							// Required Window Style
        _initX, _initY,								// Window Position
        WindowRect.right - WindowRect.left,			// Calculate Window Width
        WindowRect.bottom - WindowRect.top,			// Calculate Window Height
        NULL,										// No Parent Window
        NULL,										// No Menu
        m_hInstance,								// Instance
        NULL);
    if (!m_hAppWindow)
    {
        MessageBox(NULL, TEXT("Failed to create window"), NULL, NULL);
        return false;
    }

    return true;
}

bool D3D9Console::setWindowSize(int cx, int cy)
{
    // ֻ�ı䴰�ڴ�С
    SetWindowPos(m_hAppWindow, 0, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    return true;
}

// See http://en.wikibooks.org/wiki/DirectX/9.0/Direct3D/Initialization
bool D3D9Console::InitDirect3D()
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

 	UINT videoCardCount = m_pd3dObject->GetAdapterCount();
 	D3DADAPTER_IDENTIFIER9* videoCards = (D3DADAPTER_IDENTIFIER9*)malloc(sizeof(*videoCards)*videoCardCount);
 	for (UINT i = 0; i < videoCardCount; i++)
 	{
 		//printf("GetAdapterIdentifier()\n");
 		m_pd3dObject->GetAdapterIdentifier(i, 0, &videoCards[i]);
 	}
 
 	// ��ӡ�Կ���Ϣ
 	//printf("Adapter List:\n");
 	for (UINT i = 0; i < videoCardCount; i++)
 	{
 		//printf("%s\n", videoCards[i].DeviceName);
		printf("%s\n", videoCards[i].Description);
 	}

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
    if (FAILED(m_pd3dObject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
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
    m_d3dPresent.hDeviceWindow = m_hAppWindow;				// ����ģʽ�£�Ĭ��(NULL)Ϊ�����������豸ʱָ����focus window

    // Create the D3dDevice
    hr = m_pd3dObject->CreateDevice(D3DADAPTER_DEFAULT, devicetype, m_hAppWindow, vtx_proc, &m_d3dPresent, &m_pDevice);
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
}

void D3D9Console::Shutdown()
{
    lua_close(L);
    ReleaseDeviceResources();
    SafeDelete(&m_pTimer);
}

void D3D9Console::CalculateFPS(float dt)
{
    static int frameCnt;
    static float elapsedTime;

    frameCnt++;
    elapsedTime += dt;

    if (elapsedTime >= 1.0f)
    {
        m_fps = (float)frameCnt / elapsedTime;
        frameCnt = 0;
        elapsedTime = 0;
    }
}

// �豸��ʧ�������豸
// �˷���������ȷ���豸��ʧ����ܵ���
void D3D9Console::TryResetDevice()
{
    HRESULT hr;
    while (FAILED(hr = m_pDevice->TestCooperativeLevel()))
    {
        // �豸��������
        if (D3DERR_DEVICENOTRESET == hr)
        {
            // �ͷž���
            m_pSprite->OnLostDevice();

            // �����豸״̬
            if (SUCCEEDED(hr = m_pDevice->Reset(&m_d3dPresent)))
            {
				m_pSprite->OnResetDevice();
				this->ResetGraphicsState();
            }
        } 
        //else if (D3DERR_DRIVERINTERNALERROR == hr)
        //{
        //    // Destroy and recreate device
        //    MessageBox(NULL, TEXT("Direct3D driver internal error."), NULL, NULL);
        //    exit(1);
        //}

        // wait a moment and try again
        Sleep(100);
    }

    m_bDeviceLost = false;
}

LRESULT D3D9Console::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        {
            //m_width = LOWORD(lParam);
            //m_height = HIWORD(lParam);
            //this->ResetGraphicsState();
        } break;
    case WM_LBUTTONDOWN:
        {
            POINT pos;
            pos.x = LOWORD(lParam);
            pos.y = HIWORD(lParam);
            lua_getglobal(L, "mousepressed");
            lua_pushinteger(L, pos.x);
            lua_pushinteger(L, pos.y);
            lua_pushstring(L, "left");
            lua_pcall(L, 3, 0, 0);
        } break;
    case WM_RBUTTONDOWN:
        {
            POINT pos;
            pos.x = LOWORD(lParam);
            pos.y = HIWORD(lParam);
            lua_getglobal(L, "mousepressed");
            lua_pushinteger(L, pos.x);
            lua_pushinteger(L, pos.y);
            lua_pushstring(L, "right");
            lua_pcall(L, 3, 0, 0);
        } break;
    case WM_CHAR:
        {
            // �Ƿ��ظ�����
            WORD isrepeat = HIWORD(lParam) & KF_REPEAT;
            lua_getglobal(L, "keypressed");
            lua_pushstring(L, ctos(wParam));
            lua_pushboolean(L, isrepeat);
            lua_pcall(L, 2, 0, 0);
        } break;
    default:
        result = DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return result;
}

double D3D9Console::get_frame_rate() const
{
    return m_fps;
}