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

static void init_module()
{
    ps.image_mode = CALYX_IMAGE_MODE_CORNER;
}

static D3D9Application* thisApp(lua_State *L)
{
    lua_getfield(L, LUA_REGISTRYINDEX, "thisapp");
    D3D9Application* self = (D3D9Application*)lua_touserdata(L, -1);
    // TODO throw exception
    assert(self != NULL);
    return self;
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

static const struct luaL_reg processinglib[] =
{
	//{"scale", scale},
	//{"rotate", rotate},
	//{"pushMatrix", getsize},
	//{"popMatrix", getsize},
	//{"frameRate", getsize},
	//{"popMatrix", getsize},
    {"loadImage", l_loadImage},
    {"image",     l_image},
	{"imageMode", l_imageMode},
    {"text",      l_text},
	{NULL,        NULL}
};

extern "C" int luaopen_calyx_processing(lua_State *L)
{
    init_module();
	luaL_openlib(L, "processing", processinglib, 0);
	return 1;
}