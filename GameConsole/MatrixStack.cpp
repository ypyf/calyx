#include "MatrixStack.h"

using namespace calyx;

MatrixStack::MatrixStack()
{
    D3DXCreateMatrixStack(0, &m_stack);
}

MatrixStack::~MatrixStack()
{

}

void MatrixStack::loadIdentity()
{
    m_stack->LoadIdentity();
}

void MatrixStack::loadMatrix(D3DXMATRIX* mat)
{
    m_stack->LoadMatrix(mat);
}

D3DXMATRIX* MatrixStack::top()
{
    return m_stack->GetTop();
}

void MatrixStack::push()
{
    m_stack->Push();
}

void MatrixStack::pop()
{
    m_stack->Pop();
}

// �ø����ľ�����˵�ǰ����
// �����ü���Ľ���滻��ǰ����
void MatrixStack::multMatrix(const D3DXMATRIX* mat)
{
    m_stack->MultMatrixLocal(mat);
}

void MatrixStack::scale(double x, double y, double z)
{
    m_stack->ScaleLocal((FLOAT)x, (FLOAT)y, (FLOAT)z);
}

void MatrixStack::translate(double x, double y, double z)
{
    m_stack->TranslateLocal((FLOAT)x, (FLOAT)y, (FLOAT)z);
}