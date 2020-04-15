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
    void init_module(lua_State* L)
    {
    }

    int new_graphics(lua_State* L) {
        Graphics g;
        return create_userdata(L, &g, sizeof(Graphics));
    }

    // TODO 返回更多信息
    int get_videocard_info(lua_State* L) {
        D3D9Console* console = D3D9Console::GetThis(L);
        VideoCardInfo info = console->GetVideoCardInfo();
        lua_pushstring(L, info.Description.c_str());
        return 1;
    }

    const struct luaL_reg exports[] =
    {
        {"new",       new_graphics},
        {"getVideoCardInfo", get_videocard_info},
        {NULL,          NULL}
    };
}

int luaopen_calyx_graphics(lua_State* L)
{
    init_module(L);

    // calyx.graphics = {}
    lua_getglobal(L, "calyx");
    lua_newtable(L);
    lua_setfield(L, 2, "graphics");

    // calyx.graphics.getVideoCardInfo = get_videocard_info
    lua_getfield(L, -1, "graphics");
    lua_pushcfunction(L, get_videocard_info);
    lua_setfield(L, -2, "getVideoCardInfo");

    return 1;
}