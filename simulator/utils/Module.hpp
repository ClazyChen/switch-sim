#pragma once

namespace fpga::utils {
    
    // 用于模拟一个硬件模块的抽象
    struct Module {

        // 应当在 Module 中定义 IO 子类以描述模块的输入输出

        // 重置模块，默认不做任何事情
        // 用于模拟 reset 信号的作用
        // 对于不包含 RegInit 的模块，reset 信号不发生作用
        // 而是等待输入信号的变化，然后更新输出
        virtual void reset() { 
            // do nothing
        }

        // 更新模块的输出（组合逻辑）
        virtual void update() {
            // do nothing
        }
        
        // 运行模块（模拟一个时钟周期）
        virtual void run() {
            // do nothing
        }

        // 对于每个周期的功能，总是先 update 再 run
        void tick() {
            update();
            run();
        }

    };
}