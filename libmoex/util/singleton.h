#ifndef SINGLETON_H
#define SINGLETON_H

#include "../node/common.h"


MOEX_NAMESPACE_BEGIN

template <typename T>
class Singleton{
public:
    template <typename... Args>
    static T& Instance(Args&&... args){
        static T instance(std::forward<Args>(args)...);
        return instance;
    }
};


template <typename T>
class SingletonPtr{
public:
    template <typename... Args>
    static T* Instance(Args&&... args){
        if(instance_ == nullptr)
            instance_ = new T(std::forward<Args>(args)...);
        return instance_;
    }

    static T* GetInstance(){
        if(instance_ == nullptr)
            throw std::logic_error("this instance is not init");
        return instance_;
    }

    static void DestroyInstance(){
        if(instance_ == nullptr)
            return;
        delete instance_;
        instance_ = nullptr;
    }
private:
    SingletonPtr();
    virtual ~SingletonPtr();
    SingletonPtr(const SingletonPtr &);
    SingletonPtr& operator = (const SingletonPtr &);
private:
    static T* instance_;
};

template <typename T>
T* SingletonPtr<T>::instance_ = nullptr;

MOEX_NAMESPACE_END

#endif // SINGLETON_H
