#pragma once

#include <windows.h> // This has to be included before any D3d header file
#include <TCHAR.h>

#include "Timer.h"
#include "stringUtils.h"
#include "basic_types.h"
#include "keyboard.h"
#include "TexCube.h"
#include "MatrixStack.h"
#include "Console.h"

struct lua_State;

namespace calyx
{

class D3D9Console : public Console
{
public:
    D3D9Console();
    virtual ~D3D9Console();

    // Main event loop
    int Run();

    // create the app window
    bool CreateConsoleWindow(int cx, int cy);

    // Framework methods
    virtual int Init();
    virtual void Update(float dt);
    virtual void Draw();
    virtual LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    virtual void Pause(bool status);
    virtual void Quit();

    // 设置摄像机
    // TODO 抽象出Camera对象
    void SetCameraAtZ(double z);

    // 从lua虚拟机获取控制台实例
    static D3D9Console *GetThis(lua_State *L);

    double GetFrameRate() const;

    ID3DXFont *m_d3dxFont;
    MatrixStack m_matrixStack;
    IDirect3DDevice9 *m_d3d9Device;
    ID3DXSprite *m_pSprite;
    D3DCOLOR m_d3dColorBackground; // 背景色
    ID3DXMesh *m_pTeapotMesh;      // 茶壶网格

    // 显卡信息
    D3DADAPTER_IDENTIFIER9 m_d3d9Adapter;

    operator HWND() const;

protected:
    // Attributes
    HWND m_hAppWindow;
    HINSTANCE m_hInstance;
    bool m_bSuspended; // 暂停更新画面

    // 窗口标题
    cstring m_appCaption;

    // Direct3D
    IDirect3D9 *m_d3d9;
    D3DPRESENT_PARAMETERS m_d3dPresent;
    D3DFORMAT m_d3dVertexIndexFormat;

    // 处理图形设备相关联的资源
    virtual bool AcquireDeviceResources();
    virtual void ReleaseDeviceResources();

    // 处理设备丢失
    virtual void TryResetDevice();
    //virtual bool OnResetDevice();
    //virtual bool OnLostDevice();
    bool m_bHandleDeviceLost;
    bool m_bDeviceLost;

    // 鼠标滚轮一次滚动的行数
    int m_mouseScrollDeltaPerLine;

    // Calculate FPS
    void
    CalculateFPS(float dt);

    // Shutdown the app

    virtual void Shutdown();

    // 初始化Lua脚本引擎

    int InitLua();

    // Initialize graphics device

    bool InitDirect3D();
    bool SetGraphicsDeviceState();
    bool InitGraphics();

    // 设置窗口大小
    bool setWindowSize(int cx, int cy);

private:
    double m_fps;
    Timer *m_pTimer;
    //D3DXMATRIX view;

    // lua vm
    lua_State *L;

    bool m_bLight; // 开启光照
    ID3DXMesh *m_pMesh;

    // D3D 相关数据
    D3DMATERIAL9 m_material;
    IDirect3DVertexBuffer9 *m_pVB; // 顶点缓存
    IDirect3DIndexBuffer9 *m_pIB;  // 索引缓存
};

} // namespace calyx
