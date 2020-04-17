#ifndef __d3d_util_h__
#define __d3d_util_h__

#include <d3d9.h>
#include <d3dx9.h>

#include "basic_types.h"
#include "typesystem.h"

namespace d3d
{
    class Color
    {
    public:
        static const D3DCOLOR Black = D3DCOLOR_ARGB(255, 0, 0, 0);
        static const D3DCOLOR White = D3DCOLOR_ARGB(255, 255, 255, 255);
        static const D3DCOLOR AntiqueWhite = D3DCOLOR_ARGB(255, 255, 239, 219);
        static const D3DCOLOR Floral = D3DCOLOR_ARGB(255, 255, 250, 240);
        static const D3DCOLOR Linen = D3DCOLOR_ARGB(255, 250, 240, 230);
        static const D3DCOLOR PeachPuff = D3DCOLOR_ARGB(255, 255, 218, 185);
        static const D3DCOLOR LemonChiffon = D3DCOLOR_ARGB(255, 255, 250, 205);
        static const D3DCOLOR Seashell = D3DCOLOR_ARGB(255, 255, 245, 238);
        static const D3DCOLOR Azure = D3DCOLOR_ARGB(255, 240, 255, 255);
        static const D3DCOLOR Lavender = D3DCOLOR_ARGB(255, 230, 230, 250);
        static const D3DCOLOR MistyRose = D3DCOLOR_ARGB(255, 255, 228, 225);
        static const D3DCOLOR DimGrey = D3DCOLOR_ARGB(255, 105, 105, 105);
        static const D3DCOLOR SlateGrey = D3DCOLOR_ARGB(255, 112, 128, 144);
        static const D3DCOLOR Grey = D3DCOLOR_ARGB(255, 190, 190, 190);
        static const D3DCOLOR Navy = D3DCOLOR_ARGB(255, 0, 0, 128);
        static const D3DCOLOR Cornflower = D3DCOLOR_ARGB(255, 100, 149, 237);	// 矢车菊 XNA's default clear color
        static const D3DCOLOR Aqua = D3DCOLOR_ARGB(255, 127, 255, 212);	// 海蓝宝石
        static const D3DCOLOR SlateBlue = D3DCOLOR_ARGB(255, 106, 90, 205);
        static const D3DCOLOR Medium = D3DCOLOR_ARGB(255, 0, 0, 205);
        static const D3DCOLOR Royal = D3DCOLOR_ARGB(255, 65, 105, 225);
        static const D3DCOLOR Blue = D3DCOLOR_ARGB(255, 0, 0, 255);
        static const D3DCOLOR Dodger = D3DCOLOR_ARGB(255, 30, 144, 255);
        static const D3DCOLOR DeepSky = D3DCOLOR_ARGB(255, 0, 191, 255);
        static const D3DCOLOR Red = D3DCOLOR_ARGB(255, 255, 0, 0);
        static const D3DCOLOR DeepPink = D3DCOLOR_ARGB(255, 255, 20, 147);
        static const D3DCOLOR Green = D3DCOLOR_ARGB(255, 0, 255, 0);
        static const D3DCOLOR Olive = D3DCOLOR_ARGB(255, 107, 142, 35);	// 军绿色
        static const D3DCOLOR Yellow = D3DCOLOR_ARGB(255, 255, 255, 0);
        static const D3DCOLOR Cyan = D3DCOLOR_ARGB(255, 0, 255, 255);
        static const D3DCOLOR Purple = D3DCOLOR_ARGB(255, 155, 48, 255);
        static const D3DCOLOR Lime = D3DCOLOR_ARGB(255, 50, 205, 50);
        static const D3DCOLOR Orange = D3DCOLOR_ARGB(255, 255, 165, 0);
    };

    struct VertexColor
    {
        float pos[3];
        D3DCOLOR color;
        static const DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
    };

    struct VertexTex
    {
        float pos[3];
        float u, v;
        static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
    };

    struct VertexNormTex
    {
        float pos[3];
        float norm[3];
        float u, v;
        static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
    };
}

inline void create_texture_from_file(LPDIRECT3DDEVICE9 device, LPCTSTR file, LPDIRECT3DTEXTURE9* texture)
{
    //D3DXCreateTextureFromFile(device, file, texture);
    D3DXCreateTextureFromFileEx(device,
        file,
        D3DX_DEFAULT_NONPOW2,
        D3DX_DEFAULT_NONPOW2,
        D3DX_FROM_FILE,
        0,
        D3DFMT_FROM_FILE,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0, 0, 0,
        texture);
}

//
// See http://msdn.microsoft.com/en-us/library/windows/desktop/dd370994(v=vs.85).aspx
//
template<class Interface>
inline void SafeRelease(Interface** ppInterface)
{
    if (*ppInterface != NULL)
    {
        (*ppInterface)->Release();
        (*ppInterface) = NULL;
    }
}

template <class Resource>
inline void SafeDelete(Resource** ppResource)
{
    if (*ppResource != NULL)
    {
        delete (*ppResource);
        (*ppResource) = NULL;
    }
}

#endif // __d3d_util_h__