#include "GameTime.h"

using namespace calyx;

GameTime::GameTime()
{
	m_countsPerSec = 0;
	m_secPerCount = 0;
	m_prevCounts = 0;
}

GameTime::~GameTime()
{

}

int GameTime::Init()
{
	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&m_countsPerSec))
		return false;	// 系统不支持高精度计数器

	m_secPerCount = 1.0f / m_countsPerSec;
	return true;
}

void GameTime::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&m_prevCounts);
}

void GameTime::Reset()
{
	m_prevCounts = 0;
}

// 返回在调用Start以后所过去的秒数
float GameTime::Elapsed()
{
	__int64 curCounts;
	QueryPerformanceCounter((LARGE_INTEGER*)&curCounts);
	float deltaTime = (curCounts - m_prevCounts) * m_secPerCount;
	m_prevCounts = curCounts;
	return deltaTime;
}