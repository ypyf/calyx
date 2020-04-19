// calyx.processing模块提供了Processing语言的部分API

#include "../../mylua.h"
#include "../../d3dUtil.h"
#include "../../D3D9Console.h"
#include "processing.h"

#include <assert.h>

using namespace calyx;

enum ImageMode
{
    CALYX_IMAGE_MODE_CORNER = 0,
    CALYX_IMAGE_MODE_CENTER = 1
};

// 用于保存processing状态
struct PState
{
    int image_mode;
};

struct ImageData : lua::UserData
{
    IDirect3DTexture9 *tex;
    D3DXIMAGE_INFO info;
    void GC() override
    {
        tex->Release();
    }
};

namespace
{
// 全局(模块)环境钩子
int module_env_index(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    const char *index = luaL_optstring(L, 2, "");
    if (!strcmp("frameRate", index)) // 当前帧率
    {
        lua_pushnumber(L, D3D9Console::GetThis(L)->GetFrameRate());
        return 1;
    }
    else
    {
        // TODO throw
        return luaL_error(L, "引用用了未定义的变量 '%s'\n", index);
    }
}

void init_module(lua_State *L)
{
    static PState ps;
    ps.image_mode = CALYX_IMAGE_MODE_CORNER;
    lua_pushlightuserdata(L, &ps);
    //lua_setfield(L, LUA_ENVIRONINDEX, "state ");

    // 创建模块环境元表
    // __index(table, key)
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, module_env_index);
    lua_rawset(L, -3);
    lua_setmetatable(L, LUA_ENVIRONINDEX);
}

PState *get_pstate(lua_State *L)
{
    lua_getfield(L, LUA_ENVIRONINDEX, "state");
    PState *pps = (PState *)lua_touserdata(L, -1);
    assert(pps != NULL);
    return pps;
}

int l_imageMode(lua_State *L)
{
    int args = 1;
    int mode = (int)luaL_optnumber(L, args++, 0);
    get_pstate(L)->image_mode = mode;
    return 0;
}

// 载入图片纹理，返回一个Image对象
int l_loadImage(lua_State *L)
{
    TCHAR *file = ansi_to_unicode(luaL_optstring(L, 1, ""));
    IDirect3DTexture9 *tex;
    D3DXIMAGE_INFO image_info;
    D3DXGetImageInfoFromFile(file, &image_info);
    create_texture_from_file(D3D9Console::GetThis(L)->m_d3d9Device, file, &tex);
    ImageData image;
    image.tex = tex;
    image.info = image_info;
    return create_userdata(L, &image, sizeof(ImageData));
}

// 绘制纹理
int l_image(lua_State *L)
{
    static D3DXVECTOR3 centre;   // 精灵中心点(0代表左上角)
    static D3DXVECTOR3 position; // 精灵的位置(0代表左上角)

    D3D9Console *console = D3D9Console::GetThis(L);
    int args = 1;
    ImageData *pImage = (ImageData *)lua_touserdata(L, args++);

    // 计算图片中心点距左上角的偏移
    float centerX = pImage->info.Width / 2.f;
    float cneterY = pImage->info.Height / 2.f;

    // TODO mode
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
    console->m_pSprite->SetTransform(console->m_matrixStack.top());
    console->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    console->m_pSprite->Draw(pImage->tex, NULL, &centre, &position, D3DCOLOR_RGBA(255, 255, 255, 255));
    console->m_pSprite->End();
    return 0;
}

// 输出文字
int l_text(lua_State *L)
{
    static RECT rc = {};
    int n = lua_gettop(L);
    if (n < 3)
        return 0;

    int args = 1;
    const char *utf8Str = luaL_optstring(L, args++, "");
    TCHAR *unicodeStr = ansi_to_unicode(utf8Str);
    rc.left = (int)luaL_optnumber(L, args++, 0);
    rc.top = (int)luaL_optnumber(L, args++, 0);

    // 创建字体Sprite
    // TODO 放入Game类中
    D3D9Console *console = D3D9Console::GetThis(L);
    console->m_pSprite->SetTransform(console->m_matrixStack.top());
    console->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);
    console->m_font->DrawText(console->m_pSprite, unicodeStr, -1, &rc, DT_NOCLIP, d3d::Color::White);
    console->m_pSprite->End();
    free(unicodeStr);

    return 0;
}

// 矩阵栈操作 -------------------

// TODO 检查push深度
int l_pushMatrix(lua_State *L)
{
    D3D9Console::GetThis(L)->m_matrixStack.push();
    return 0;
}

int l_popMatrix(lua_State *L)
{
    D3D9Console::GetThis(L)->m_matrixStack.pop();
    return 0;
}

// rotate(angle)
int l_rotate(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    if (n == 1)
    {
        // 角度在0到2pi之间
        double angle = luaL_optnumber(L, args++, 0.0);
        D3DXMATRIX Rz;
        D3DXVECTOR3 v(0, 0, 1.0f);
        // 默认绕Z轴旋转
        //D3DXMatrixRotationAxis(&Rz, &v, (FLOAT)angle);
        D3DXMatrixRotationZ(&Rz, (FLOAT)angle);
        D3D9Console::GetThis(L)->m_matrixStack.multMatrix(&Rz);
    }
    return 0;
}

int l_scale(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Console *console = D3D9Console::GetThis(L);
    if (n == 1)
    {
        double s = luaL_optnumber(L, args++, 0.0);
        console->m_matrixStack.scale(s, s, s);
    }
    else if (n == 2)
    {
        double x, y;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        console->m_matrixStack.scale(x, y, 1.0);
    }
    else if (n >= 3)
    {
        double x, y, z;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        z = luaL_optnumber(L, args++, 0.0);
        console->m_matrixStack.scale(x, y, z);
    }
    else
    {
        console->m_matrixStack.scale(1.0, 1.0, 1.0);
    }
    return 0;
}

int l_translate(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Console *console = D3D9Console::GetThis(L);
    /*    if (n == 1) {
        double x;
        x = luaL_optnumber(L, args++, 0.0);
        self->m_matrixStack.translate(x, 0.0, 0.0);
        } else*/
    if (n == 2)
    {
        double x, y;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        console->m_matrixStack.translate(x, y, 0.0);
    }
    else if (n >= 3)
    {
        double x, y, z;
        x = luaL_optnumber(L, args++, 0.0);
        y = luaL_optnumber(L, args++, 0.0);
        z = luaL_optnumber(L, args++, 0.0);
        console->m_matrixStack.translate(x, y, z);
    }
    else
    {
        console->m_matrixStack.translate(0.0, 0.0, 0.0);
    }
    return 0;
}

// 设置背景色
// 有下列重载形式
// bg(rgb)
// bg(num, num)
// bg(r, g, b)
// bg(r, g, b, a)
// bg(image), image:string
// TODO 目前只考虑了RGB颜色模式
int l_background(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Console *console = D3D9Console::GetThis(L);

    // 如果参数只有一个，则存在两种算法
    // 一种是将一个整数按位分解为RGB三个分量
    // 另一种是Processing中采用的，将这个整数作为三个相同的分量
    if (n == 1)
    {
        int v = (int)luaL_optnumber(L, args++, 0);
        // integer => (r, g, b)
        //int red = (rgb >> 16) & 0xFF;
        //int green = (rgb >> 8) & 0xFF;
        //int blue = rgb & 0xFF;
        console->m_bgcolor = D3DCOLOR_ARGB(255, v, v, v);
    }
    else if (n == 3)
    {
        // RGB分量 0~255
        int v1 = (int)luaL_optnumber(L, args++, 0);
        int v2 = (int)luaL_optnumber(L, args++, 0);
        int v3 = (int)luaL_optnumber(L, args++, 0);
        console->m_bgcolor = D3DCOLOR_ARGB(255, v1, v2, v3);
    }
    else if (n >= 4)
    {
        // RGBA分量 0~255
        int v1 = (int)luaL_optnumber(L, args++, 0);
        int v2 = (int)luaL_optnumber(L, args++, 0);
        int v3 = (int)luaL_optnumber(L, args++, 0);
        int alpha = (int)luaL_optnumber(L, args++, 0);
        console->m_bgcolor = D3DCOLOR_ARGB(alpha, v1, v2, v3);
    }

    return 0;
}

const struct luaL_Reg exports[] =
    {
        {"scale", l_scale},
        {"rotate", l_rotate},
        {"translate", l_translate},
        {"pushMatrix", l_pushMatrix},
        {"popMatrix", l_popMatrix},
        {"background", l_background},
        {"loadImage", l_loadImage},
        {"image", l_image},
        {"imageMode", l_imageMode},
        {"text", l_text},
        {NULL, NULL}};

} // namespace

int luaopen_processing(lua_State *L)
{
    init_module(L);
    luaL_openlib(L, "processing", exports, 0);
    return 1;
}
