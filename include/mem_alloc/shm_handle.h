﻿#pragma once
#include <cstddef>

#ifdef _WIN32
#include "shm_win.h"
#else
#include "shm_linux.h"
#endif

namespace smd {

class ShmHandle {
public:
	ShmHandle(Log& log)
		: m_shm(log) {}

	void* acquire(int shm_key, std::size_t size, ShareMemOpenMode mode) {
		return m_shm.acquire(shm_key, size, mode);
	}

	void release() { m_shm.release(); }

private:
#ifdef _WIN32
	ShmWin m_shm;
#else
	ShmLinux m_shm;
#endif
};

} // namespace smd
