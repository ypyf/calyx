// calyx.processing模块提供了Processing语言的部分API

#include "processing.h"
#include "../../d3dUtil.h"

#define CALYX_IMAGE_MODE_CORNER 0
#define CALYX_IMAGE_MODE_CENTER 1

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

static const struct luaL_reg processinglib[] =
{
	//{"text", l_text},
	//{"scale", scale},
	//{"rotate", rotate},
	//{"pushMatrix", getsize},
	//{"popMatrix", getsize},
	//{"frameRate", getsize},
	//{"popMatrix", getsize},
	{"imageMode", l_imageMode},
	{NULL, NULL}
};

extern "C" int luaopen_calyx_processing(lua_State *L)
{
    init_pstate();
	luaL_openlib(L, "processing", processinglib, 0);
	return 1;
}