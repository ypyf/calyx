// calyx.processingģ���ṩ��Processing���ԵĲ���API

#include "processing.h"
#include "../../d3dUtil.h"
#include "../../Game.h"
#include <assert.h>

#define CALYX_IMAGE_MODE_CORNER 0
#define CALYX_IMAGE_MODE_CENTER 1

using namespace calyx;

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

static PState ps;

static D3D9Application* thisApp(lua_State *L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "thisapp");
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, -1);
    // TODO throw exception
    assert(self != NULL);
    return self;
}


// ȫ��(ģ��)��������
// __index(table, key)
static int module_env_index(lua_State *L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    const char* index = luaL_optstring(L, 2, "");
    if (!strcmp("frameRate", index))
    {
        D3D9Application* self = thisApp(L);
        lua_pushnumber(L, self->get_frame_rate());
        return 1;
    }
    else
    {
        // TODO throw
        return luaL_error(L, "��������δ����ı��� '%s'\n", index);
    }
}

static void init_module(lua_State *L)
{
    ps.image_mode = CALYX_IMAGE_MODE_CORNER;

    // ����ģ�黷��Ԫ��
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, module_env_index);
    lua_rawset(L, -3);
    lua_setmetatable(L, LUA_ENVIRONINDEX);
}

static int l_imageMode(lua_State *L)
{
	int args = 1;
	int mode = (int)luaL_optnumber(L, args++, 0);
	ps.image_mode = mode;
	return 0;
}

// ����ͼƬ��������һ��Image����
static int l_loadImage(lua_State *L)
{
    D3D9Application* self = thisApp(L);
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

// ��������
static int l_image(lua_State *L)
{
    static D3DXVECTOR3 centre;    // �������ĵ�(0�������Ͻ�)
    static D3DXVECTOR3 position;  // �����λ��(0�������Ͻ�)

    D3D9Application* self = thisApp(L);
    int args = 1;
    ImageData* pImage = (ImageData*)lua_touserdata(L, args++);

    // ����ͼƬ���ĵ�����Ͻǵ�ƫ��
    float centerX = pImage->info.Width / 2.f;
    float cneterY = pImage->info.Height / 2.f;

    // TODO mode
    //if (pImage->mode == CALYX_IMAGE_MODE_CORNER)
    //{
    // ʹͼƬ���ĵ������ָ����λ����
    position.x = (float)luaL_optnumber(L, args++, 0) - centerX;
    position.y = (float)luaL_optnumber(L, args++, 0) - cneterY;
    //}
    //else
    //{
    // ʹͼƬ������������ת
    //centre.x = position.x + centerX;
    //centre.y = position.y + cneterY;
    //}
    self->m_pSprite->SetTransform(self->m_matrixStack.top());       
    self->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    self->m_pSprite->Draw(pImage->tex, NULL, &centre, &position, D3DCOLOR_RGBA(255,255,255,255));  
    self->m_pSprite->End();  
    return 0;
}

// �������
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
        // ��������Sprite TODO ����Game����
        D3D9Application* self = thisApp(L);
        self->m_pSprite->SetTransform(self->m_matrixStack.top());
        self->m_pSprite->Begin(D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE | D3DXSPRITE_SORT_DEPTH_FRONTTOBACK);
        self->m_font->DrawText(self->m_pSprite, text, -1, &rc0, DT_NOCLIP, d3d::Color::White);
        self->m_pSprite->End();
        free(text);
    }
    return 0;
}

// ����ջ���� -------------------

// TODO ���push���
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
    D3D9Application* self = thisApp(L);
    if (n == 1) {
        // �Ƕ���0��2pi֮��
        double angle = luaL_optnumber(L, args++, 0.0);
        D3DXMATRIX Rz;
        D3DXVECTOR3 v(0, 0, 1.0f);
        // Ĭ����Z����ת
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
    D3D9Application* self = thisApp(L);
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
    D3D9Application* self = thisApp(L);
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

// ���ñ���ɫ
// ������������ʽ
// bg(rgb)
// ??bg(num, num)
// bg(r, g, b)
// bg(r, g, b, a)
// bg(image), image:string
// TODO Ŀǰֻ������RGB��ɫģʽ
static int l_background(lua_State *L)
{
    int n = lua_gettop(L);
    int args = 1;
    D3D9Application* self = thisApp(L);

    // �������ֻ��һ��������������㷨
    // һ���ǽ�һ��������λ�ֽ�ΪRGB��������
    // ��һ����Processing�в��õģ������������Ϊ������ͬ�ķ���
    if (n == 1) {
        int v = (int)luaL_optnumber(L, args++, 0);
        // integer => (r, g, b)
        //int red = (rgb >> 16) & 0xFF;
        //int green = (rgb >> 8) & 0xFF;
        //int blue = rgb & 0xFF;
        self->m_bgcolor = D3DCOLOR_ARGB(255, v, v, v);
    } else if (n == 3) {
        // RGB���� 0~255
        int v1 = (int)luaL_optnumber(L, args++, 0);
        int v2 = (int)luaL_optnumber(L, args++, 0);
        int v3 = (int)luaL_optnumber(L, args++, 0);
        self->m_bgcolor = D3DCOLOR_ARGB(255, v1, v2, v3);
    } else if (n >= 4) {
        // RGBA���� 0~255
        int v1 = (int)luaL_optnumber(L, args++, 0);
        int v2 = (int)luaL_optnumber(L, args++, 0);
        int v3 = (int)luaL_optnumber(L, args++, 0);
        int alpha = (int)luaL_optnumber(L, args++, 0);
        self->m_bgcolor = D3DCOLOR_ARGB(alpha, v1, v2, v3);
    }

    return 0;
}


static const struct luaL_reg processinglib[] =
{
	{"scale", l_scale},
	{"rotate", l_rotate},
    {"translate", l_translate},
	{"pushMatrix", l_pushMatrix},
	{"popMatrix", l_popMatrix},
	//{"frameRate", getsize},
    {"background", l_background},
    {"loadImage", l_loadImage},
    {"image",     l_image},
	{"imageMode", l_imageMode},
    {"text",      l_text},
	{NULL,        NULL}
};

extern "C" int luaopen_calyx_processing(lua_State *L)
{
    init_module(L);
	luaL_openlib(L, "processing", processinglib, 0);
	return 1;
}