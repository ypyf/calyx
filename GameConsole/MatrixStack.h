#ifndef __matrix_stack_h__
#define __matrix_stack_h__

#include <D3dx9math.h>
#include "typesystem.h"

namespace calyx {

// D3DX����ջ��װ��
class MatrixStack
{
public:
    MatrixStack();
    ~MatrixStack();
    D3DXMATRIX* top();   // ���ص�ǰ����
    void loadIdentity(); // �õ�λ�����滻��ǰ����
    void loadMatrix(D3DXMATRIX *pMat);    // �ø����ľ����滻��ǰ����
    void push();   // ���Ƶ�ǰ����
    void pop();   // ������ǰ����
    void multMatrix(const D3DXMATRIX *pMat);    // ����˷�
    void scale(double x, double y, double z);   // ����
	void translate(double x, double y, double z);	// ƽ��

private:
    ID3DXMatrixStack* m_stack;
	DISALLOW_COPY_AND_ASSIGN(MatrixStack);
};

}   // namespace calyx

#endif // __matrix_stack_h__