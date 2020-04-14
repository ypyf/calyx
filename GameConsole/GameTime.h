#ifndef __game_time_h__
#define __game_time_h__

#pragma once

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
        __int64 _countsPerSec;	// ÆµÂÊ
        __int64 _prevCounts;
        float	_secPerCount;
    };

} // namespace neo

#endif // __game_time_h__