﻿#pragma once

namespace Maragi
{
    template<typename T>
    class Singleton // thread-safe global
    {
    private:
        class Deleter
        {
        public:
            void operator ()(T *ptr) const
            {
                if(sizeof(T) > 0)
                    delete ptr;
            }
        };

    protected:
        ~Singleton() {}

    private:
        static std::unique_ptr<T, Deleter> ptr;
        static boost::once_flag onceFlag;

    public:
        static T &instance()
        {
            boost::call_once(onceFlag, [] { ptr.reset(new T); });
            return *ptr.get();
        }
        static T &inst() { return instance(); }
    };

    template<typename T>
    std::unique_ptr<T, typename Singleton<T>::Deleter> Singleton<T>::ptr;
    template<typename T>
    boost::once_flag Singleton<T>::onceFlag = BOOST_ONCE_INIT;

    template<typename T>
    class SingletonLocal // thread-local (using TLS)
    {
    protected:
        ~SingletonLocal() {}

    private:
        static void deleter(T *ptr)
        {
            delete ptr;
        }

    public:
        static T &instance()
        {
            static boost::thread_specific_ptr<T> ptr(deleter);
            if(ptr.get() == nullptr)
                ptr.reset(new T);
            return *ptr;
        }
        static T &inst() { return instance(); }
    };
}
