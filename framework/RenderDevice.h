#pragma once

namespace calyx
{

class RenderDevice
{
public:
    virtual int CreateDeivce() = 0;
    virtual int TryResetDevice() = 0;
    //virtual int ResetDevice() = 0;
    virtual int DestroyDevice() = 0;
    virtual int SetRenderState(int state, int value) = 0;
    //virtual int SetTransform(D3DTS_VIEW, &view) = 0;
    virtual int CreateVertexBuffer() = 0;
    virtual int CreateIndexBuffer() = 0;
};

} // namespace calyx
