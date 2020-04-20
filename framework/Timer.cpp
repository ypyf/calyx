#include "Timer.h"
#include <windows.h>

using namespace calyx;

Timer::Timer()
    : _countsPerSec(0),
      _secPerCount(0),
      _prevCounts(0)
{
}

Timer::~Timer()
{
}

int Timer::Init()
{
    if (!QueryPerformanceFrequency((LARGE_INTEGER *)&_countsPerSec))
        return false; // 系统不支持高精度计数器

    _secPerCount = 1.0f / _countsPerSec;
    return true;
}

void Timer::Start()
{
    QueryPerformanceCounter((LARGE_INTEGER *)&_prevCounts);
}

void Timer::Reset()
{
    _prevCounts = 0;
}

// 返回在调用Start以后所过去的秒数
float Timer::Elapsed()
{
    __int64 curCounts;
    QueryPerformanceCounter((LARGE_INTEGER *)&curCounts);
    float deltaTime = (curCounts - _prevCounts) * _secPerCount;
    _prevCounts = curCounts;
    return deltaTime;
}
