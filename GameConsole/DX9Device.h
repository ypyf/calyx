#ifndef DX9DEVICE_H_
#define DX9DEVICE_H_

#pragma once

#include "basic_types.h"
#include "d3dUtil.h"
#include "RenderDevice.h"

namespace calyx
{
	class DX9Device : public RenderDevice
	{
	public:
		DX9Device(HWND hDeviceWindow);
		~DX9Device();
		virtual int CreateDeivce() override;
		virtual int TryResetDevice() override;
		virtual int DestroyDevice() override;


	private:
		DISALLOW_COPY_AND_ASSIGN(DX9Device);

	private:
		HWND                    m_hDeviceWindow;
		IDirect3D9*				m_pd3dObject;
		IDirect3DDevice9*		m_pDevice;
		D3DPRESENT_PARAMETERS	m_d3dPresent;
		D3DFORMAT				m_d3dVertexIndexFormat;
	};

}	// namespace calyx

#endif	/* DX9DEVICE_H_ */