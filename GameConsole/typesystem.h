#ifndef _TYPE_SYSTEM_H_
#define _TYPE_SYSTEM_H_

// 表示派生类覆盖了基类中的虚函数
#define OVERRIDE

#define BITS_PER_BYTE	8

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned int uint32;
typedef unsigned char uint8;
typedef unsigned short uint16;

typedef int32 atomic32;

#if defined(WIN32)
#define atomicIncrement(refcount) InterlockedIncrement(refcount)
#define atomicDecrement(refcount)	InterlockedDecrement(refcount)
#endif // WIN32

// 不可复制的类
// 请在类定义中的private:声明后使用这个宏
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
    void operator=(const TypeName&)

// 不可显式构造的类
// 隐藏构造函数
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName();                                    \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

#endif /* _TYPE_SYSTEM_H_ */