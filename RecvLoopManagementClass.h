#pragma once
#include <thread>
#include <mutex>
class RecvLoopManagementClass
{
public:
	std::thread m_recvLoop;

public:
	RecvLoopManagementClass();
	
	inline bool IsOff() const	{ std::lock_guard<std::mutex> locker(m_offMutex);	return m_isOff; }
	inline bool IsPause() const	{ std::lock_guard<std::mutex> locker(m_pauseMutex);	return m_isPause; }

	inline void SetOff(bool off)		{ std::lock_guard<std::mutex> locker(m_offMutex);	m_isOff = off; }
	inline void SetPause(bool pause)	{ std::lock_guard<std::mutex> locker(m_pauseMutex);	m_isPause = pause; }
	inline void On()	{ SetOff(false); }
	inline void Off()	{ SetOff(true); }
	inline void Run()	{ SetPause(false); }
	inline void Pause()	{ SetPause(true); }

private:
	bool m_isOff;
	bool m_isPause;
	mutable std::mutex m_offMutex;
	mutable std::mutex m_pauseMutex;
};

