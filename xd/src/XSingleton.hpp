#ifndef _X_SINGLETON_HPP
#define _X_SINGLETON_HPP


template<class T>
class XSingleton
{
public:
    static T& instance()
    {
        static T obj;
        return obj;
    }

private:

    XSingleton(){}
    XSingleton(const XSingleton<T>&){}
    XSingleton<T>& operator=(const XSingleton<T>&){}
};

#endif //_X_SINGLETON_HPP
