#pragma once

#include "Mau.hpp"

namespace fpga::p4 {

    // 用于实现整条流水线的控制
    // N: 流水线阶段数
    template <size_t N>
    struct Pipeline : public Module {
        struct IO {
            Pipe pipe;
        } io;

        // 实现 N 个流水线阶段
        std::array<std::unique_ptr<Module>, N> stages;

    private:

        // 每个流水线阶段的输入输出端口
        std::array<Pipe*, N> pipes;

        // 初始化每个流水线阶段为 Mau<I>
        template <size_t I> requires (I >= 0 && I < N)
        void init() {
            auto mau = std::make_unique<Mau<I>>();
            pipes[I] = &mau->io.pipe;
            stages[I] = std::move(mau);
            if constexpr (I < N - 1) {
                init<I + 1>();
            }
        }

    public:
        Pipeline() {
            init<0>();
        }

        void reset() override {
            io.pipe.reset();
            for (auto& stage : stages) {
                stage->reset();
            }
        }

        void update() override {
            // 传入每一级流水线的 input
            io.pipe.input_to(*pipes[0]);
            
            // 需要注意的是 update 只用于更新每一级的 input 并完成其内部组合逻辑
            // 但不会将 output 传递到下一级，所以先传入 input 再 update
            for (size_t i = 0; i < N - 1; i++) {
                pipes[i]->transport_to(*pipes[i + 1]);
            }

            // 运行每一级流水线的 update
            for (auto& stage : stages) {
                stage->update();
            }

        }

        void run() override {
            // 运行每一级流水线的 run
            for (auto& stage : stages) {
                stage->run();
            }

            // 传出最后一级流水线的 output
            pipes[N - 1]->output_to(io.pipe);
        }
        

    };

}