#include <stdio.h>
#include <assert.h>
//#include <WinInet.h>	// URL
#include "Game.h"
#include "Calyx.h"
#include "modules.h"
#include "MyModels.h"

namespace
{
    calyx::D3D9Application* g_pApp;

    // non-member window procedure
    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        return g_pApp->WindowProc(hWnd, msg, wParam, lParam);
    }
}

using namespace calyx;

D3D9Application::D3D9Application()
{

}

D3D9Application::~D3D9Application()
{

}

// 全局环境钩子
// __index(table, key)
static int global_env_hook(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    const char* index = luaL_optstring(L, 2, "");
    if (!strcmp("frameRate", index))
    {
        D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
        lua_pushnumber(L, self->m_fps);
    }
    else
    {
        // TODO 有好的错误处理
        printf("error: 引用用了未定义的变量 %s\n", index);
        lua_pushnil(L);
    }
    return 1;
}

// 获取帧率
static int calyx_lua_frameRate(lua_State *L)
{
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    lua_pushnumber(L, self->m_fps);  /* push result */
    return 1;  /* number of results */
}

#define CALYX_IMAGE_MODE_CORNER	0
#define CALYX_IMAGE_MODE_CENTER 1

struct ImageData : UserData
{
    IDirect3DTexture9* tex;
    D3DXIMAGE_INFO info;
    void GC() override
    {
        tex->Release();
    }
};

// 载入图片纹理，返回一个Image对象
static int calyx_lua_loadImage(lua_State *L)
{
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    TCHAR* file = ansi_to_unicode(luaL_optstring(L, 1, ""));
    IDirect3DTexture9* tex;
    D3DXIMAGE_INFO image_info;
    D3DXGetImageInfoFromFile(file, &image_info);
    create_texture_from_file(self->m_pDevice, file, &tex);
    ImageData image;
    image.tex = tex;
    image.info = image_info;
    return create_userdata(L, &image, sizeof(ImageData));
}

// 绘制纹理
static int calyx_lua_image(lua_State *L)
{
    static D3DXVECTOR3 centre;    // 精灵中心点(0代表左上角)
    static D3DXVECTOR3 position;  // 精灵的位置(0代表左上角)

    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    int args = 1;
    ImageData* pImage = (ImageData*)lua_touserdata(L, args++);

    // 计算图片中心点距左上角的偏移
    float centerX = pImage->info.Width / 2.f;
    float cneterY = pImage->info.Height / 2.f;

    //if (pImage->mode == CALYX_IMAGE_MODE_CORNER)
    //{
    // 使图片中心点出现在指定的位置上
    position.x = (float)luaL_optnumber(L, args++, 0) - centerX;
    position.y = (float)luaL_optnumber(L, args++, 0) - cneterY;
    //}
    //else
    //{
    // 使图片绕自身中心旋转
    //centre.x = position.x + centerX;
    //centre.y = position.y + cneterY;
    //}
    self->m_pSprite->SetTransform(self->m_matrixStack.top());       
    self->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    self->m_pSprite->Draw(pImage->tex, NULL, &centre, &position, D3DCOLOR_RGBA(255,255,255,255));  
    self->m_pSprite->End();  
    return 0;
}

// 设置背景色
// 有下列重载形式
// bg(rgb)
// ??bg(num, num)
// bg(r, g, b)
// bg(r, g, b, a)
// bg(image), image:string
// TODO 目前只考虑了RGB颜色模式
static int calyx_lua_background(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));

    // 如果参数只有一个，则存在两种算法
    // 一种是将一个整数按位分解为RGB三个分量
    // 另一种是Processing中采用的，将这个整数作为三个相同的分量
    if (n == 1) {
        int v = (int)luaL_optnumber(L, args++, 0);
        // integer => (r, g, b)
        //int red = (rgb >> 16) & 0xFF;
        //int green = (rgb >> 8) & 0xFF;
        //int blue = rgb & 0xFF;
        self->m_bgcolor = D3DCOLOR_ARGB(255, v, v, v);
    } else if (n == 3) {
        // RGB分量 0~255
        int v1 = (int)luaL_optnumber(L, args++, 0);
        int v2 = (int)luaL_optnumber(L, args++, 0);
        int v3 = (int)luaL_optnumber(L, args++, 0);
        self->m_bgcolor = D3DCOLOR_ARGB(255, v1, v2, v3);
    } else if (n >= 4) {
        // RGBA分量 0~255
        int v1 = (int)luaL_optnumber(L, args++, 0);
        int v2 = (int)luaL_optnumber(L, args++, 0);
        int v3 = (int)luaL_optnumber(L, args++, 0);
        int alpha = (int)luaL_optnumber(L, args++, 0);
        self->m_bgcolor = D3DCOLOR_ARGB(alpha, v1, v2, v3);
    }

    return 0;
}

// 矩阵栈操作 -------------------

// TODO 检查push深度
static int calyx_lua_pushMatrix(lua_State *L)
{
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    self->m_matrixStack.push();
    return 0;
}

static int calyx_lua_popMatrix(lua_State *L)
{
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    self->m_matrixStack.pop();
    return 0;
}

// rotate(angle)
static int calyx_lua_rotate(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    if (n == 1) {
        // 角度在0到2pi之间
        double angle = luaL_optnumber(L, args++, 0.0);
        D3DXMATRIX Rz;
        D3DXVECTOR3 v(0, 0, 1.0f);
        // 默认绕Z轴旋转
        //D3DXMatrixRotationAxis(&Rz, &v, (FLOAT)angle);
        D3DXMatrixRotationZ(&Rz, (FLOAT)angle);
        self->m_matrixStack.multMatrix(&Rz);
    }
    return 0;
}

static int calyx_lua_scale(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    if (n == 1) {
        double s = luaL_optnumber(L, args++, 0.0);
        self->m_matrixStack.scale(s, s, s);
    } else if (n == 2) {
        double x, y;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        self->m_matrixStack.scale(x, y, 1.0);
    } else if (n >= 3) {
        double x, y, z;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        z = luaL_optnumber(L, args++, 0.0);
        self->m_matrixStack.scale(x, y, z);
    } else {
        self->m_matrixStack.scale(1.0, 1.0, 1.0);
    }
    return 0;
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
        // 创建窗口
        self->CreateD3D9Window(cx, cy);
    }
    // TODO 错误处理

    return 0;
}
#endif

static int calyx_lua_translate(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
    /*    if (n == 1) {
    double x;
    x = luaL_optnumber(L, args++, 0.0);
    self->m_matrixStack.translate(x, 0.0, 0.0);
    } else*/ if (n == 2) {
        double x, y;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        self->m_matrixStack.translate(x, y, 0.0);
    } else if (n >= 3) {
        double x, y, z;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        z = luaL_optnumber(L, args++, 0.0);
        self->m_matrixStack.translate(x, y, z);
    } else {
        self->m_matrixStack.translate(0.0, 0.0, 0.0);
    }
    return 0;
}

// 输出文字
static int calyx_lua_processing_text(lua_State *L)
{
    static RECT rc0 = {0, 0, 0, 0};
    int n = lua_gettop(L);
    int args = 1;
    if (n >= 3) {
        TCHAR *text = ansi_to_unicode(luaL_optstring(L, args++, ""));
        int x = (int)luaL_optnumber(L, args++, 0);
        int y = (int)luaL_optnumber(L, args++, 0);
        rc0.left = x;
        rc0.top = y;
        // 创建字体Sprite TODO 放入Game类中
        D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
        self->m_pSprite->SetTransform(self->m_matrixStack.top());
        self->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);
        self->m_font->DrawText(self->m_pSprite, text, -1, &rc0, DT_NOCLIP, d3d::Color::White);
        self->m_pSprite->End();
        free(text);
    }
    return 0;
}

// 事件处理程序
// TODO 事件队列
static int calyx_lua_event_post(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    if (n >= args) {
        std::string event = luaL_optstring(L, args++, "");
        D3D9Application* self = (D3D9Application*)lua_touserdata(L, lua_upvalueindex(1));
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

static int calyx_lua_printf(lua_State *L) 
{
    lua_pushvalue(L, lua_upvalueindex(2));
    lua_insert(L, 1);
    lua_call(L, lua_gettop(L) - 1, 1);
    lua_pushvalue(L, lua_upvalueindex(1));
    lua_pushvalue(L, -2);
    lua_call(L, 1, 0);
    return 0;
}


int D3D9Application::InitLua()
{
    L = luaL_newstate();
    luaL_openlibs(L);

    // 预加载核心模块
    // 安装内建模块预载入过程
    init_modules(L);

    // 指示引擎当前在独立模式下运行（.exe模式）
    //{
    //    lua_pushboolean(L, 1);
    //    lua_setfield(L, -2, "standalone");
    //}

    // 运行启动脚本
    // TODO 作为子模块预载入，然后在此处require
    luaL_dofile(L, "boot.lua");

    // 获取窗口标题和大小信息
    // TODO 让LuaRuntime去解析这个嵌套的表
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

    // 截断过长的标题
    m_sAppTitle = m_sAppTitle.substr(0, 128);

    // 加入printf函数
    lua_getglobal(L, "io");
    lua_getglobal(L, "string");
    lua_pushliteral(L, "write");
    lua_gettable(L, -3);
    lua_pushliteral(L, "format");
    lua_gettable(L, -3);
    lua_pushcclosure(L, calyx_lua_printf, 2);
    lua_setglobal(L, "printf");

    // 事件管理系统
    // event.post = c_event_post
    lua_getglobal(L, "calyx");
    lua_getfield(L, -1, "event");
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_event_post, 1);
    lua_setfield(L, -2, "post");    

    // 图形方面的函数-------------------
    // TODO 用数组和循环

    lua_newtable(L);
    lua_setglobal(L, "screen");

    // 屏幕分辨率
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    lua_getglobal(L, "screen");
    lua_pushinteger(L, screen_width);
    lua_setfield(L, -2, "width");

    lua_getglobal(L, "screen");
    lua_pushinteger(L, screen_height);
    lua_setfield(L, -2, "height");

    // 设置窗口大小
    //lua_pushlightuserdata(L, this);
    //lua_pushcclosure(L, calyx_lua_size, 1);
    //lua_setglobal(L, "size");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_processing_text, 1);
    lua_setglobal(L, "text");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_loadImage, 1);
    lua_setglobal(L, "loadImage");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_image, 1);
    lua_setglobal(L, "image");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_pushMatrix, 1);
    lua_setglobal(L, "pushMatrix");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_popMatrix, 1);
    lua_setglobal(L, "popMatrix");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_popMatrix, 1);
    lua_setglobal(L, "popMatrix"); 

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_scale, 1);
    lua_setglobal(L, "scale"); 

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_rotate, 1);
    lua_setglobal(L, "rotate");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_translate, 1);
    lua_setglobal(L, "translate");

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, calyx_lua_background, 1);
    lua_setglobal(L, "background");

    // 设置全局环境元表
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushlightuserdata(L, this);	// 保存this指针
    lua_pushcclosure(L, global_env_hook, 1);
    lua_rawset(L, -3);
    lua_setmetatable(L, LUA_GLOBALSINDEX);

    return 1;
}

int D3D9Application::Run()
{
    // 载入脚本
    int ret = luaL_dofile(L, "main.lua");
    if (ret != 0)
    {
        TCHAR message[1024];
        wsprintf(message, TEXT("%s\n"), ansi_to_unicode(lua_tostring(L,-1)));
        MessageBox(NULL, message, TEXT("Calyx"), MB_OK|MB_ICONSTOP);
        return false;
    }

    // 调用入口函数
    lua_getglobal(L, "setup");
    if (lua_pcall(L, 0, 0, 0) != 0)
        return false;

    // 处理系统消息
    m_pTimer->Start();
    MSG msg = {0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) 
        {
            TranslateMessage(&msg);	// 翻译虚拟键码为ASCII
            DispatchMessage(&msg);
        }
        else
        {
            //// 处理Lua事件
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

void D3D9Application::Update(float dt)
{
    lua_getglobal(L, "update");
    lua_pushnumber(L, dt);
    lua_pcall(L, 1, 0, 0);
}

// 立方体模型
D3DXVECTOR4 cube_vertices[] = {
    // AD
    D3DXVECTOR4(0.05f, 0.0f, 0.45f, 1.0f), 
    D3DXVECTOR4(0.05f, 0.0f, 0.35f, 1.0f),
    // DC
    D3DXVECTOR4(0.05f, 0.0f, 0.35f, 1.0f),
    D3DXVECTOR4(0.15f, 0.0f, 0.35f, 1.0f),
    // CB
    D3DXVECTOR4(0.15f, 0.0f, 0.35f, 1.0f),
    D3DXVECTOR4(0.15f, 0.0f, 0.45f, 1.0f),
    // BA
    D3DXVECTOR4(0.15f, 0.0f, 0.45f, 1.0f),
    D3DXVECTOR4(0.05f, 0.0f, 0.45f, 1.0f),
    // AE
    D3DXVECTOR4(0.05f, 0.0f, 0.45f, 1.0f),
    D3DXVECTOR4(0.05f, 0.1f, 0.45f, 1.0f),
    // EF
    D3DXVECTOR4(0.05f, 0.1f, 0.45f, 1.0f),
    D3DXVECTOR4(0.15f, 0.1f, 0.45f, 1.0f),
    // FB
    D3DXVECTOR4(0.15f, 0.1f, 0.45f, 1.0f),
    D3DXVECTOR4(0.15f, 0.0f, 0.45f, 1.0f),
    // EH
    D3DXVECTOR4(0.05f, 0.1f, 0.45f, 1.0f),
    D3DXVECTOR4(0.05f, 0.1f, 0.35f, 1.0f),
    // HG
    D3DXVECTOR4(0.05f, 0.1f, 0.35f, 1.0f),
    D3DXVECTOR4(0.15f, 0.1f, 0.35f, 1.0f),
    // GF
    D3DXVECTOR4(0.15f, 0.1f, 0.35f, 1.0f),
    D3DXVECTOR4(0.15f, 0.1f, 0.45f, 1.0f),
    // GC
    D3DXVECTOR4(0.15f, 0.1f, 0.35f, 1.0f),
    D3DXVECTOR4(0.15f, 0.0f, 0.35f, 1.0f),
    // HD
    D3DXVECTOR4(0.05f, 0.1f, 0.35f, 1.0f),
    D3DXVECTOR4(0.05f, 0.0f, 0.35f, 1.0f)
};

// 绘图函数
void D3D9Application::Draw()
{
    // 处理设备丢失
    if (m_bHandleDeviceLost)
    {
        TryResetDevice();
    }

    // 清屏
    m_pDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_bgcolor, 1.0f, 0);

    // 调用绘图方法
    m_pDevice->BeginScene();

    // 重置变换矩阵
    m_matrixStack.loadIdentity();

    lua_getglobal(L, "draw");
    lua_pcall(L, 0, 0, 0);

    // 绘制立方体
    //m_pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 12, cube_vertices, sizeof(D3DXVECTOR4));

    // 结束场景绘制
    m_pDevice->EndScene();

    // 检查设备是否丢失
    if (D3DERR_DEVICELOST == m_pDevice->Present(0, 0, 0, 0))
    {
        m_bDeviceLost = true;
    }
}

bool D3D9Application::AcquireDeviceResources()
{
    m_pDevice->CreateVertexBuffer(24 * sizeof(d3d::VertexNormTex), 0, 
        d3d::VertexNormTex::FVF, D3DPOOL_MANAGED, &m_pVB, NULL);

    m_pDevice->CreateIndexBuffer(36 * sizeof(DWORD),
        D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL);

    return true;
}

void D3D9Application::ReleaseDeviceResources()
{
    SafeRelease(&m_pVB);
    SafeRelease(&m_pIB);
    SafeRelease(&m_pSprite);
}

bool D3D9Application::OnLostDevice()
{
    m_pSprite->OnLostDevice();
    return true;
}

// 进行设备丢失后的恢复
bool D3D9Application::OnResetDevice()
{
    m_pSprite->OnResetDevice();
    ResetGraphicsState();
    return true;
}

// 设置图形设备状态
bool D3D9Application::ResetGraphicsState()
{
    // 创建并设置视图矩阵
    D3DXVECTOR3 eye (1.5f, 0.75f, -1); //近乎于iso
    //D3DXVECTOR3 eye (1.5f, 0, -1);
    D3DXVECTOR3 target (0, 0, 0);
    D3DXVECTOR3 up (0, 1, 0);
    D3DXMATRIXA16 view;

    D3DXMatrixLookAtLH(&view, &eye, &target, &up);
    m_pDevice->SetTransform(D3DTS_VIEW, &view);

    // 创建并设置投影矩阵
    RECT rect;
    GetClientRect(m_hAppWindow, &rect);
    float aspect = (float)(rect.right - rect.left) / (rect.bottom - rect.top);
    D3DXMATRIX proj;
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4, aspect, 1.0f, 1000.0f);
    //D3DXMatrixOrthoLH(&proj, 5.0f, 5.0f * aspect, 0.0f, 5.0f);
    m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

    m_pDevice->SetRenderState(D3DRS_LIGHTING, 1);	// 开启光照
    m_pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);	// 线性插值着色
    m_pDevice->SetRenderState(D3DRS_NORMALIZENORMALS, 1);	// 开启单位化法线向量
    m_pDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, 1); // 开启抗锯齿

    return true;
}

// 初始化图形子系统
bool D3D9Application::InitGraphics()
{
    // 从配置中读取窗口客户区大小
    // TODO  考虑把它移入其它函数
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

    // 创建窗口
    CreateApplicationWindow(cx, cy);

    // 显示窗口
    ShowWindow(m_hAppWindow, SW_SHOW);

    // 初始化D3D
    InitDirect3D();

    // 创建图形设备资源
    if (!AcquireDeviceResources())
        return false;

    // 重置图形设备状态
    ResetGraphicsState();

    // 创建精灵
    D3DXCreateSprite(m_pDevice, &m_pSprite);

    // 创建缺省字体
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

void D3D9Application::Pause(bool status)
{
    m_bSuspended = status;
}

void D3D9Application::Quit()
{
    ::PostQuitMessage(0);
}

int D3D9Application::Init(HINSTANCE hInstance)
{
    m_bWireFrame = false;
    m_bLight = true;
    m_bgcolor = d3d::Color::Cornflower; // 缺省背景色
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

    // 初始化Lua并启动脚本
    if (!InitLua())
        return false;

    if (!InitGraphics())
        return false;

    //InitInput();
    //InitGUI();

    return true;
}

bool D3D9Application::CreateApplicationWindow(int cx, int cy)
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

    // 缺省的窗口位置
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

    m_hAppWindow = CreateWindowEx(dwExStyle,			// Extended Style For The Window
        window_class_name,						// Class Name
        title_buf,										// Window Title
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

bool D3D9Application::setWindowSize(int cx, int cy)
{
    // 只改变窗口大小
    SetWindowPos(m_hAppWindow, 0, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOZORDER);
    return true;
}

// See http://en.wikibooks.org/wiki/DirectX/9.0/Direct3D/Initialization
bool D3D9Application::InitDirect3D()
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
    // 	// 打印视频卡信息
    // 	printf("Adapter List:\n");
    // 	for (UINT i = 0; i < videoCardCount; i++)
    // 	{
    // 		printf("%s\n", videoCards[i].DeviceName);
    // 	}

    // 3. 检查设备类型
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
    // 		// 对于全屏程序，DisplayFormat与BackBufferForamt必须相同
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

    // 检查硬件变换和光照
    DWORD vtx_proc;
    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
        vtx_proc = D3DCREATE_HARDWARE_VERTEXPROCESSING;
        if (caps.DevCaps & D3DDEVCAPS_PUREDEVICE) {
            // 这个特性能提高速度，但不利于调试
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

    // 5.1 获取当前的显示模式
    D3DDISPLAYMODE d3ddm;
    if (FAILED(m_pd3dObject->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm)))
        return false;

    ZeroMemory(&m_d3dPresent, sizeof(m_d3dPresent));
    m_d3dPresent.Windowed = windowed;						// 窗口模式
    //m_d3dPresent.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;	// 立即交换, 默认值为能使得交换帧率等于屏幕刷新率
    m_d3dPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;		//该模式可以搭配任何MultiSampleType(D3DMULTISAMPLE_NONE甚至只能使用该效果)
    m_d3dPresent.BackBufferFormat = d3ddm.Format;			// 在窗口模式下，默认值为(0)使用当前显示模式
    m_d3dPresent.BackBufferCount = 1;						// 后备缓存区个数
    m_d3dPresent.EnableAutoDepthStencil = TRUE;
    m_d3dPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_d3dPresent.MultiSampleType = D3DMULTISAMPLE_NONE;		// 如果SwapEffect不是D3DSWAPEFFECT_DISCARD时，必须填D3DMULTISAMPLE_NONE
    m_d3dPresent.hDeviceWindow = m_hAppWindow;				// 窗口模式下，默认(NULL)为接下来创建设备时指定的focus window

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
    // 设置视口(非必须)
    D3DVIEWPORT9 vp = { 20, 20, 640, 480, 0, 1 };
    m_pDevice->SetViewport(&vp);
#endif

    return true;
}

void D3D9Application::Shutdown()
{
    lua_close(L);
    ReleaseDeviceResources();
    SafeDelete(&m_pTimer);
}

void D3D9Application::CalculateFPS(float dt)
{
    static int frameCnt;
    static float elapsedTime;
    //static TCHAR buffer[256];

    frameCnt++;
    elapsedTime += dt;

    if (elapsedTime >= 1.0f)
    {
        m_fps = (float)frameCnt / elapsedTime;
        // Reset elapsed time and frame counter
        frameCnt = 0;
        elapsedTime = 0;
    }
}

// 设备丢失后重置设备
void D3D9Application::TryResetDevice()
{
    HRESULT hr;

    if (m_bDeviceLost)
    {
        while (FAILED(hr = m_pDevice->TestCooperativeLevel()))
        {
            // 设备可以重置
            if (D3DERR_DEVICENOTRESET == hr)
            {
                // 释放精灵
                this->OnLostDevice();

                //Reset the device
                if (SUCCEEDED(hr = m_pDevice->Reset(&m_d3dPresent)))
                {
                    this->OnResetDevice();
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
}

LRESULT D3D9Application::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
            // 是否重复击键
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
