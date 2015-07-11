// calyx.processing模块提供了Processing语言的部分API

#include "processing.h"
#include "../../d3dUtil.h"
#include "../../D3D9Console.h"
#include <assert.h>

enum ImageMode
{
	CALYX_IMAGE_MODE_CORNER = 0,
	CALYX_IMAGE_MODE_CENTER = 1
};

using namespace calyx;

// 用于保存processing状态
struct PState
{
    int image_mode;
};

struct ImageData : UserData
{
    IDirect3DTexture9* tex;
    D3DXIMAGE_INFO info;
    void GC() override
    {
        tex->Release();
    }
};

static D3D9Console* thisApp(lua_State *L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "thisapp");
    D3D9Console* self = (D3D9Console*)lua_touserdata(L, -1);
    // TODO throw exception
    assert(self != NULL);
    return self;
}

// 全局(模块)环境钩子
static int module_env_index(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    const char* index = luaL_optstring(L, 2, "");
    if (!strcmp("frameRate", index))	// 当前帧率
    {
        D3D9Console* self = thisApp(L);
        lua_pushnumber(L, self->get_frame_rate());
        return 1;
    }
    else
    {
        // TODO throw
        return luaL_error(L, "引用用了未定义的变量 '%s'\n", index);
    }
}

static void init_module(lua_State *L)
{
    static PState ps;
    ps.image_mode = CALYX_IMAGE_MODE_CORNER;
    lua_pushlightuserdata(L, &ps);
    lua_setfield(L, LUA_ENVIRONINDEX, "state");

    // 创建模块环境元表
	// __index(table, key)
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, module_env_index);
    lua_rawset(L, -3);
    lua_setmetatable(L, LUA_ENVIRONINDEX);
}

static PState* get_pstate(lua_State *L)
{
	lua_getfield(L, LUA_ENVIRONINDEX, "state");
	PState* pps = (PState*)lua_touserdata(L, -1);
	assert(pps != NULL);
	return pps;
}

static int l_imageMode(lua_State *L)
{
	int args = 1;
	int mode = (int)luaL_optnumber(L, args++, 0);
	get_pstate(L)->image_mode = mode;
	return 0;
}

// 载入图片纹理，返回一个Image对象
static int l_loadImage(lua_State *L)
{
    D3D9Console* self = thisApp(L);
    assert(self != NULL);
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
static int l_image(lua_State *L)
{
    static D3DXVECTOR3 centre;    // 精灵中心点(0代表左上角)
    static D3DXVECTOR3 position;  // 精灵的位置(0代表左上角)

    D3D9Console* self = thisApp(L);
    int args = 1;
    ImageData* pImage = (ImageData*)lua_touserdata(L, args++);

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
    self->m_pSprite->SetTransform(self->m_matrixStack.top());       
    self->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    self->m_pSprite->Draw(pImage->tex, NULL, &centre, &position, D3DCOLOR_RGBA(255,255,255,255));  
    self->m_pSprite->End();  
    return 0;
}

// 输出文字
static int l_text(lua_State *L)
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
        D3D9Console* self = thisApp(L);
        self->m_pSprite->SetTransform(self->m_matrixStack.top());
        self->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);
        self->m_font->DrawText(self->m_pSprite, text, -1, &rc0, DT_NOCLIP, d3d::Color::White);
        self->m_pSprite->End();
        free(text);
    }
    return 0;
}

// 矩阵栈操作 -------------------

// TODO 检查push深度
static int l_pushMatrix(lua_State *L)
{
    thisApp(L)->m_matrixStack.push();
    return 0;
}

static int l_popMatrix(lua_State *L)
{
    thisApp(L)->m_matrixStack.pop();
    return 0;
}

// rotate(angle)
static int l_rotate(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Console* self = thisApp(L);
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

static int l_scale(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Console* self = thisApp(L);
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

static int l_translate(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Console* self = thisApp(L);
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

// 设置背景色
// 有下列重载形式
// bg(rgb)
// ??bg(num, num)
// bg(r, g, b)
// bg(r, g, b, a)
// bg(image), image:string
// TODO 目前只考虑了RGB颜色模式
static int l_background(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Console* self = thisApp(L);

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


static const struct luaL_reg exports[] =
{
    {"scale",       l_scale},
    {"rotate",      l_rotate},
    {"translate",   l_translate},
    {"pushMatrix",  l_pushMatrix},
    {"popMatrix",   l_popMatrix},
    {"background",  l_background},
    {"loadImage",   l_loadImage},
    {"image",       l_image},
    {"imageMode",   l_imageMode},
    {"text",        l_text},
    {NULL,          NULL}
};

extern "C" int luaopen_calyx_processing(lua_State *L)
{
    init_module(L);
	luaL_openlib(L, "processing", exports, 0);
	return 1;
}