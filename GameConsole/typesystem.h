#ifndef _TYPE_SYSTEM_H_
#define _TYPE_SYSTEM_H_

// ��ʾ�����า���˻����е��麯��
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

// ���ɸ��Ƶ���
// �����ඨ���е�private:������ʹ�������
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&);               \
    void operator=(const TypeName&)

// ������ʽ�������
// ���ع��캯��
#define DISALLOW_IMPLICIT_CONSTRUCTORS(TypeName) \
    TypeName();                                    \
    DISALLOW_COPY_AND_ASSIGN(TypeName)

#endif /* _TYPE_SYSTEM_H_ */