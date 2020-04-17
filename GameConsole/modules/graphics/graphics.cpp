#include <assert.h>

#include "graphics.h"
#include "../../mylua.h"
#include "../../d3dUtil.h"
#include "../../D3D9Console.h"

using namespace calyx;

struct Graphics : lua::UserData {
    // 显卡描述信息
    std::string videoCardDesc;
    void GC() override
    {
        printf("lua: 回收Graphics对象\n");
    }
};

namespace {
    int new_graphics(lua_State* L) {
        Graphics g;
        return create_userdata(L, &g, sizeof(Graphics));
    }

    // TODO 返回更多信息
    int l_get_videocard_info(lua_State* L) {
        D3D9Console* console = D3D9Console::GetThis(L);
        VideoCardInfo info = console->GetVideoCardInfo();
        lua_pushstring(L, info.Description.c_str());
        return 1;
    }

    // 绘制茶壶
    int l_teapot(lua_State* L) {
        D3D9Console* console = D3D9Console::GetThis(L);
        console->m_pTeapotMesh->DrawSubset(0);
        return 0;
    }

    const struct luaL_reg exports[] =
    {
        {"new",       new_graphics},
        {"getVideoCardInfo", l_get_videocard_info},
        {"teapot", l_teapot},
        {NULL,          NULL}
    };
}

int init_calyx_graphics(lua_State* L)
{
    // calyx.graphics = {}
    lua_getglobal(L, "calyx");
    lua_newtable(L);
    lua_setfield(L, -2, "graphics");

    // calyx.graphics.getVideoCardInfo = get_videocard_info
    lua_getfield(L, -1, "graphics");
    lua_pushcfunction(L, l_get_videocard_info);
    lua_setfield(L, -2, "getVideoCardInfo");

    lua_pushcfunction(L, l_teapot);
    lua_setfield(L, -2, "teapot");

    return 1;
}