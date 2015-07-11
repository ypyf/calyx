#ifndef __texcube_h__
#define __texcube_h__

#include "d3dUtil.h"
#include "stringUtils.h"

class TexCube
{
public:
	TexCube(IDirect3DDevice9* pDevice, D3DXVECTOR3 pos, D3DXVECTOR3 scale, D3DXVECTOR3 rot, const TCHAR* texfile);
	~TexCube();

	int SetupGraphics();
	void Update(float dt);
	void Draw() const;

	/*D3DXMATRIX& GetWorldMatrix() const;*/

	bool CreateDeviceResources();
	void DiscardDeviceResources();
	bool RestoreDeviceResources();

private:

	IDirect3DDevice9* m_pDevice;

	//Vectors
	D3DXVECTOR3	m_pos;
	D3DXVECTOR3	m_scale;
	D3DXVECTOR3	m_rot;

	//Matrix
	D3DXMATRIX	m_world;

	//Vertex and index buffers
	IDirect3DVertexBuffer9* m_pVB;
	IDirect3DIndexBuffer9*	m_pIB;

	//Texture
	IDirect3DTexture9*	m_pTexture;
	cstring m_sTexFile;
	
};

#endif // __texcube_h__