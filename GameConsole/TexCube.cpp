#include "TexCube.h"

// 贴图
//(0,0)-----(0,1)
//
//
//
//(1,0)-----(1,1)
namespace
{
	d3d::VertexTex verts[8] =
	{
		// Front
		{-0.25f, 0.25f, -0.25f, 0.0f, 0.0f},
		{0.25f, 0.25f, -0.25f, 0.0f, 1.0f},
		{-0.25f, -0.25f, -0.25f, 1.0f, 0.0f},
		{0.25f, -0.25f, -0.25f, 1.1f, 1.1f},

		// Back
		{-0.25f, 0.25f, 0.25f, 0.0f, 0.0f},
		{0.25f, 0.25f, 0.25f, 0.0f, 1.0f},
		{-0.25f, -0.25f, 0.25f, 1.0f, 0.0f},
		{0.25f, -0.25f, 0.25f, 1.1f, 1.1f},
	};

	WORD indices[36] =
	{
		// Front face
		0, 1, 2,
		1, 3, 2,

		// Back face
		5, 4, 7,
		7, 4, 6,

		// Right face
		1, 5, 3,
		3, 5, 7,

		// Left face
		4, 0, 6,
		6, 0, 2,

		// Top face
		4, 5, 0,
		0, 5, 1,

		// Bottom face
		2, 3, 6,
		6, 3, 7,
	};
}

TexCube::TexCube(IDirect3DDevice9* pDevice, D3DXVECTOR3 pos, D3DXVECTOR3 scale, D3DXVECTOR3 rot, const TCHAR* texfile)
{
	m_pDevice = pDevice;
	m_pos = pos;
	m_scale = scale;
	m_rot = rot;
	m_sTexFile = texfile;
	m_pTexture = NULL;
}

TexCube::~TexCube()
{

}

int TexCube::SetupGraphics()
{
	// 拷贝顶点数据到缓存
	VOID* pVerts;
	m_pVB->Lock(0, sizeof(verts), (void**)&pVerts, 0);
	memcpy(pVerts, &verts, sizeof(verts));
	m_pVB->Unlock();

	// 拷贝索引数据到缓存
	VOID* pIndices;
	m_pIB->Lock(0, sizeof(indices), (void**)&pIndices, 0);
	memcpy(pIndices, &indices, sizeof(indices));
	m_pIB->Unlock();

	m_pDevice->SetStreamSource(0, m_pVB, 0, sizeof(d3d::VertexTex));
	m_pDevice->SetFVF(d3d::VertexTex::FVF);
	m_pDevice->SetIndices(m_pIB);
	m_pDevice->SetTexture(0, m_pTexture);

	return true;
}

void TexCube::Update(float dt)
{

}

void TexCube::Draw() const
{
	m_pDevice->SetTransform(D3DTS_WORLD, &m_world);
	m_pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 36, 0, 12);
}

bool TexCube::CreateDeviceResources()
{
	m_pDevice->CreateVertexBuffer(NELEM(verts) * sizeof(d3d::VertexColor), 0, 
		d3d::VertexColor::FVF, D3DPOOL_MANAGED, &m_pVB, NULL);

	m_pDevice->CreateIndexBuffer(NELEM(indices) * sizeof(DWORD),
		D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, NULL);

	//Load texture
	D3DXCreateTextureFromFile(m_pDevice, TEXT("carving.png"), &m_pTexture);
	if (!m_pTexture)
	{
		MessageBox(NULL, TEXT("Failed to create texture"), NULL, NULL);
		return false;
	}

	return true;
}

void TexCube::DiscardDeviceResources()
{
	SafeRelease(&m_pVB);
	SafeRelease(&m_pIB);
	SafeRelease(&m_pTexture);
}

bool TexCube::RestoreDeviceResources()
{
	if (!CreateDeviceResources())
		return false;

	SetupGraphics();
	return true;
}