#include "../mylua.h"
#include "../calyx.h"
#include "../D3D9Console.h"

using namespace calyx;

namespace
{
// TODO 返回更多信息
int l_get_videocard_info(lua_State *L)
{
    D3D9Console *console = D3D9Console::GetThis(L);
    lua_pushstring(L, console->m_d3d9Adapter.Description);
    return 1;
}

// 绘制茶壶
int l_teapot(lua_State *L)
{
    D3D9Console *console = D3D9Console::GetThis(L);
    console->m_pTeapotMesh->DrawSubset(0);
    return 0;
}

// 设置线框填充模式
int l_set_wireframe(lua_State *L)
{
    D3D9Console *console = D3D9Console::GetThis(L);
    if (lua_isboolean(L, 1) && lua_toboolean(L, 1))
    {
        console->m_d3d9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    }
    else
    {
        console->m_d3d9Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    }
    return 0;
}

int l_is_wireframe(lua_State *L)
{
    D3D9Console *console = D3D9Console::GetThis(L);
    DWORD value;
    console->m_d3d9Device->GetRenderState(D3DRS_FILLMODE, &value);
    lua_pushboolean(L, value == D3DFILL_WIREFRAME);
    return 1;
}

int l_set_camera_z(lua_State *L)
{
    if (lua_gettop(L) == 1 && lua_isnumber(L, 1))
    {
        D3D9Console *console = D3D9Console::GetThis(L);
        console->SetCameraAtZ(lua_tonumber(L, 1));
    }
    return 0;
}

const struct luaL_Reg exports[] =
    {
        {"getVideoCardInfo", l_get_videocard_info},
        {"teapot", l_teapot},
        {"setWireframe", l_set_wireframe},
        {"isWireframe", l_is_wireframe},
        {"setCameraZ", l_set_camera_z},
        {NULL, NULL}};
} // namespace

int init_calyx_graphics(lua_State *L)
{
    // calyx.graphics = {}
    lua_getglobal(L, "calyx");
    lua_newtable(L);
    lua_setfield(L, 1, "graphics");

    // load calyx.graphics
    lua_getfield(L, 1, "graphics");

    for (size_t i = 0; exports[i].name != NULL; i++)
    {
        lua_pushcfunction(L, exports[i].func);
        lua_setfield(L, -2, exports[i].name);
    }

    lua_pop(L, 1);

    return TRUE;
}
