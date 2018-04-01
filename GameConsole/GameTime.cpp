#include "GameTime.h"
#include <windows.h>

using namespace calyx;

GameTime::GameTime()
	: _countsPerSec(0),
	  _secPerCount(0),
      _prevCounts(0)
{

}

GameTime::~GameTime()
{

}

int GameTime::Init()
{
	if(!QueryPerformanceFrequency((LARGE_INTEGER*)&_countsPerSec))
		return false;	// ϵͳ��֧�ָ߾��ȼ�����

	_secPerCount = 1.0f / _countsPerSec;
	return true;
}

void GameTime::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&_prevCounts);
}

void GameTime::Reset()
{
	_prevCounts = 0;
}

// �����ڵ���Start�Ժ�����ȥ������
float GameTime::Elapsed()
{
	__int64 curCounts;
	QueryPerformanceCounter((LARGE_INTEGER*)&curCounts);
	float deltaTime = (curCounts - _prevCounts) * _secPerCount;
	_prevCounts = curCounts;
	return deltaTime;
}