#ifndef __sprite_batch_h__
#define __sprite_batch_h__

namespace calyx {
    // 绘制精灵
    class SpriteBatch
    {
    public:
        SpriteBatch(GraphicsDevice& _device, Console& _console)
            : device(_device), surf(0), console(_console)
        {}

        void Begin()
        {
            surf = (int32*)device.lock();
        }

        void End()
        {
            device.unlock();
            device.copy(console);
        }

        // 测试接口，只能绘制纯色
        void Draw()
        {
            for (int i = 0; i < ImageWidth * ImageHeight; i++)
            {
                surf[i] = 0xcccccccc;
            }
        }

    private:
        int32* surf;
        GraphicsDevice& device;
        Console& console;
    };
} // namespace neo

#endif // __sprite_batch_h__