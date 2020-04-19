#pragma once

namespace calyx
{

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
    __int64 _countsPerSec; // 频率
    __int64 _prevCounts;
    float _secPerCount;
};

} // namespace calyx
