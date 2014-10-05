// calyx.processing模块提供了Processing语言的部分API

#include "processing.h"
#include "../../d3dUtil.h"
#include "../../Game.h"

#define CALYX_IMAGE_MODE_CORNER 0
#define CALYX_IMAGE_MODE_CENTER 1

using namespace calyx;

struct PState
{
    int image_mode;
};

struct ImageData
{
	IDirect3DTexture9 *tex;
	D3DXIMAGE_INFO info;
	void GC()
	{
		tex->Release();
	}
};

static PState ps;

static void init_pstate()
{
    ps.image_mode = CALYX_IMAGE_MODE_CORNER;
}

static int l_imageMode(lua_State *L)
{
	int args = 1;
	int mode = (int)luaL_optnumber(L, args++, 0);
	ps.image_mode = mode;
	return 0;
}

// 载入图片纹理，返回一个Image对象
static int l_loadImage(lua_State *L)
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

static const struct luaL_reg processinglib[] =
{
	//{"text", l_text},
	//{"scale", scale},
	//{"rotate", rotate},
	//{"pushMatrix", getsize},
	//{"popMatrix", getsize},
	//{"frameRate", getsize},
	//{"popMatrix", getsize},
    {"loadImage", l_loadImage},
	{"imageMode", l_imageMode},
	{NULL, NULL}
};

extern "C" int luaopen_calyx_processing(lua_State *L)
{
    init_pstate();
	luaL_openlib(L, "processing", processinglib, 0);
	return 1;
}