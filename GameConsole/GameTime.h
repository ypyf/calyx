#ifndef __game_time_h__
#define __game_time_h__

#pragma once

#include <windows.h>

namespace calyx {

class GameTime
{
public:
	GameTime();
	~GameTime();

	int Init();
	void Start();
	void Reset();
	float Elapsed();

private:
	__int64 m_countsPerSec;	// ÆµÂÊ
	__int64 m_prevCounts;
	float	m_secPerCount;
};

} // namespace neo

#endif // __game_time_h__