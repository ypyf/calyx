#pragma once

namespace calyx
{

class Timer
{
public:
    Timer();
    ~Timer();

    int Init();
    void Start();
    void Reset();
    float Elapsed();

private:
    __int64 _countsPerSec; // 频率
    __int64 _prevCounts;
    float _secPerCount;
};

} // namespace calyx
