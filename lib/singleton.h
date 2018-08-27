#ifndef SINGLETON_H__
#define SINGLETON_H__

#include <mutex>

template<class T>
class Singleton
{
public:
	static T* getInstance()
	{
		std::call_once(init_flag, Singleton::get);
		return (T*)_instance;
	}

protected:
	Singleton() {}
private:
	static void get()
	{
		_instance = new T;
	}
private:
	static volatile T* _instance;
	static std::once_flag init_flag;
};

template<typename T> volatile T* Singleton<T>::_instance = NULL;
template<typename T> std::once_flag Singleton<T>::init_flag;
#endif
