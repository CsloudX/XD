#ifndef X_DEBUG_HPP
#define X_DEBUG_HPP

#include <new>
#include <string>
#include <iostream>


inline void* operator new(std::size_t size, const char* file,int line)
{
    std::cout<<"new: "<<file<<": "<<line<<std::endl;

    void* pMem = malloc(size);
    if(pMem)
    {

    }
    return pMem;
}
inline void* operator new[](std::size_t size, const char* file,int line)
{
    return operator new(size,file,line);
}

inline void* operator new(size_t size)
{
        return operator new(size, "<Unknown>", 0);
}

inline void* operator new[](size_t size)
{
        return operator new(size);
}

inline void* operator new(size_t size, const std::nothrow_t&) throw()
{
        return operator new(size);
}

inline void* operator new[](size_t size, const std::nothrow_t&) throw()
{
        return operator new[](size);
}

//void operator delete(void* pointer, const char* file, int line)
//{

//}
//void operator delete[](void* pointer, const char* file, int line)
//{

//}

#define new X_DEBUG_NEW
#define X_DEBUG_NEW new(__FILE__, __LINE__)

#endif //X_DEBUG_HPP
