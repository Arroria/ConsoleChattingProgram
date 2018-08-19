#pragma once

template <typename Class_t>
class Singleton
{
public:
	Singleton() {}
	virtual ~Singleton() {}

public:
	static Class_t* SingletonInstance()
	{
		if (!m_singleton)
			m_singleton = new Class_t;
		return m_singleton;
	}
	static void SingletonRelease()
	{
		if (m_singleton)
		{
			delete m_singleton;
			m_singleton = nullptr;
		}
	}

private:
	static Class_t* m_singleton;
};

template <typename Class_t>
Class_t* Singleton<Class_t>::m_singleton = nullptr;

#define SingletonInst(Class_t) (Singleton<Class_t>::SingletonInstance())
