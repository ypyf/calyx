#ifndef __basic_types_h__
#define __basic_types_h__

#pragma once

// �ر��ڳ�ʼ���б��г�ʼ����ľ���
#pragma warning(disable:4351)

//#include "reflection.h"
//#include "NonCopyable.h"

#define BITS_PER_BYTE	8

typedef int color_t;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned int uint32;
typedef unsigned char uint8;
typedef unsigned short uint16;

// ArraySizeHelper��һ����������Ϊchar[N]�ĺ���,���β�����Ϊ T[N].
// ����û��Ҫʵ��, ��Ϊsizeofֻ��Ҫ����.
template<typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(arr)����array�����Ԫ�ظ���. �ñ��ʽ�Ǳ���ʱ����,
// �������ڶ����µ�����. �������һ��ָ��ᱨ����ʱ����.
//
// ���в������arraysize(arr)��֧���������ͺ��ں����ж��������.
// ��������¾ͱ���ʹ�÷����Ͱ�ȫ��ARRAYSIZE_UNSAFE()��. ������
// C++ģ��������Ƶ�, �Ժ��ȡ��.
#define NELEM(array) (sizeof(ArraySizeHelper(array)))

// �ַ�ת�ַ���
inline const char* ctos(char c)
{
    static const char* table[128] =
    {"", "", "", "", "", "", "", "", 
    "", "", "", "", "", "", "", "", 
    "", "", "", "", "", "", "", "", 
    "", "", "", "escape", "", "", "", "", 
    " ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    ":", ";", "<", "=", ">", "?", "@",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", 
    "[", "\\", "]", "^", "_", "`",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", 
    "{", "|", "}", "~", ""};

	return (c >= 0 && c <= 127) ? table[c] : "";
}

// ����

//struct Rect
//{
//	int x; // ���ϽǺ�����
//	int y; // ���Ͻ�������
//	int height;
//	int width;
//};

// 2D��
typedef struct
{
	float x;
	float y;
} Point2D;

// ����
struct Complex
{
	double re;
	double im;

public:
	Complex operator * (const Complex &other) const
	{
		Complex c;
		c.re = this->re * other.re - this->im * other.im;
		c.im = this->im * other.re + this->re * other.im;
		return c;
	}

	Complex operator + (const Complex &other) const
	{
		Complex c;
		c.re = this->re + other.re;
		c.im = this->im + other.im;
		return c;
	}
};

// ���ɸ��Ƶ���
// ����: ��ֹ������Ŀ������캯���͸�ֵ������
// �÷�: �������ŵ����˽�г�Ա����
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

// �����Զ��������
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
	TypeName();                                    \
	DISALLOW_COPY_AND_ASSIGN(TypeName)

// ������������
#define	SINGLETON(type)	\
	static type& Instance()	\
	{	\
		static type instance;	\
		return instance;	\
	}	\
private:	\
	type(){};	\
	type(type const&);	\
	type& operator=(type const&)

#endif	// __basic_types_h__
