#ifndef __basic_types_h__
#define __basic_types_h__

#pragma once

// 关闭在初始化列表中初始数组的警告
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

// ArraySizeHelper是一个返回类型为char[N]的函数,其形参类型为 T[N].
// 函数没必要实现, 因为sizeof只需要类型.
template<typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// arraysize(arr)返回array数组的元素个数. 该表达式是编译时常量,
// 可以用于定义新的数组. 如果传递一个指针会报编译时错误.
//
// 美中不足的是arraysize(arr)不支持匿名类型和在函数中定义的类型.
// 这种情况下就必须使用非类型安全的ARRAYSIZE_UNSAFE()宏. 这是由
// C++模板机制限制的, 以后会取消.
#define NELEM(array) (sizeof(ArraySizeHelper(array)))

// 字符转字符串
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

// 矩形

//struct Rect
//{
//	int x; // 左上角横坐标
//	int y; // 左上角纵坐标
//	int height;
//	int width;
//};

// 2D点
typedef struct
{
	float x;
	float y;
} Point2D;

// 复数
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

// 不可复制的类
// 功能: 禁止调用类的拷贝构造函数和赋值操作符
// 用法: 将这个宏放到类的私有成员区域
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

// 不可自动构造的类
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
	TypeName();                                    \
	DISALLOW_COPY_AND_ASSIGN(TypeName)

#endif	// __basic_types_h__
