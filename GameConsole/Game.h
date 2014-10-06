#ifndef __d3d9_application_h__
#define __d3d9_application_h__

#define _CRT_NON_CONFORMING_SWPRINTFS

#include <windows.h>	// This has to be included before any D3d header file
#include <TCHAR.h>
#include "d3dUtil.h"
#include "MatrixStack.h"
#include "GameTime.h"
#include "stringUtils.h"
#include "basic_types.h"
#include "keyboard.h"
#include "TexCube.h"

struct lua_State;

namespace calyx {

class D3D9Application
{
public:
	D3D9Application();
	virtual ~D3D9Application();

	// Main event loop
	int Run();

    // create the app window
    bool CreateApplicationWindow(int cx, int cy);

	// Framework methods
	virtual int Init(HINSTANCE hInstance);
	virtual void Update(float dt);
	virtual void Draw();
	virtual LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    virtual void Pause(bool status);
	virtual void Quit();

    ID3DXFont *m_font;
    MatrixStack m_matrixStack;
    IDirect3DDevice9* m_pDevice;
    ID3DXSprite *m_pSprite;
    double m_fps;
    D3DCOLOR m_bgcolor; // 背景色

protected:
	// Attributes
	HWND		m_hAppWindow;
	HINSTANCE	m_hInstance;
	bool		m_bSuspended;	// 暂停更新画面
    int  m_width;   // 窗口宽度
    int  m_height;  // 窗口高度
	//int			m_uiClientWidth;
	//int			m_uiClientHeight;
	TString   m_sAppTitle;

	// Direct3D Attributes
	IDirect3D9*				m_pd3dObject;
	D3DPRESENT_PARAMETERS	m_d3dPresent;
	D3DFORMAT				m_d3dVertexIndexFormat;
    
    // 处理图形设备相关联的资源
	virtual bool AcquireDeviceResources();
	virtual void ReleaseDeviceResources();
	// 处理设备丢失
	virtual void TryResetDevice();
    virtual bool OnResetDevice();
    virtual bool OnLostDevice();
	bool m_bHandleDeviceLost;
	bool m_bDeviceLost;
	
	// Calculate FPS

	void CalculateFPS(float dt);
	
	// Shutdown the app

	virtual void Shutdown();

    // 初始化Lua脚本引擎

    int InitLua();

	// Initialize graphics device

	bool InitDirect3D();
    bool ResetGraphicsState();
    bool InitGraphics();

    // 设置窗口大小
    bool setWindowSize(int cx, int cy);

private:
	GameTime* m_pTimer;
    //D3DXMATRIX view;
    
    // lua stuff
    lua_State* L;
    //int m_refThis;  // 保存在Lua注册表中的this指针的引用

    bool m_bWireFrame;
    bool m_bLight;
    ID3DXMesh* m_pMesh;
    
    D3DMATERIAL9	m_material;
    IDirect3DVertexBuffer9* m_pVB;	// 顶点缓存
    IDirect3DIndexBuffer9*	m_pIB;	// 索引缓存
};

}	// namespace neo

#endif // __d3d9_application_h__