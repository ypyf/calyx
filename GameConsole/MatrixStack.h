#ifndef __matrix_stack_h__
#define __matrix_stack_h__

#include <D3dx9math.h>
#include "typesystem.h"

namespace calyx {

// D3DX矩阵栈包装器
class MatrixStack
{
public:
    MatrixStack();
    ~MatrixStack();
    D3DXMATRIX* top();   // 返回当前矩阵
    void loadIdentity(); // 用单位矩阵替换当前矩阵
    void loadMatrix(D3DXMATRIX *pMat);    // 用给定的矩阵替换当前矩阵
    void push();   // 复制当前矩阵
    void pop();   // 弹出当前矩阵
    void multMatrix(const D3DXMATRIX *pMat);    // 矩阵乘法
    void scale(double x, double y, double z);   // 缩放
	void translate(double x, double y, double z);	// 平移

private:
    ID3DXMatrixStack* m_stack;
	DISALLOW_COPY_AND_ASSIGN(MatrixStack);
};

}   // namespace calyx

#endif // __matrix_stack_h__