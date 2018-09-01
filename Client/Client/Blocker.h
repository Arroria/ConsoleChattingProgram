#pragma once
#include <mutex>
class Blocker
{
public:
	Blocker();
	~Blocker();

public:
	inline bool IsBlocking() const { RegionLocker locker(m_limitor); return m_isBlocking; }
	inline bool Blocking(bool on) { RegionLocker locker(m_limitor); m_isBlocking = on; }
	inline bool On() { Blocking(true); }
	inline bool Off() { Blocking(false); }

private:
	bool m_isBlocking;
	mutable std::mutex m_limitor;
};

