#pragma once

#include "Module.hpp"
#include "Wire.hpp"
#include <array>
#include <algorithm>

namespace fpga::utils {

    // 用于实现一个延迟输出的小模块
    template <typename T, int cycles, template<typename> typename I = Wire>
    requires (cycles > 0)
    struct Delay : public Module {

        struct IO {
            I<T>& in;
            Wire<T>& out;
        } io;

        // 用来存储每个阶段的结果
        std::array<Wire<T>, cycles - 1> wires;

        Delay(I<T>& in, Wire<T>& out):
            io{in, out} {
            // 直接连接输入输出（采用引用）
        }
        
        // 重置模块接口
        void reset() override {
            // 没有任何效果
        }

        // 组合逻辑接口
        void update() override {
            // 没有任何效果
        }

        // 时序逻辑接口
        void run() override {
            if constexpr (cycles == 1) {
                // 如果只有一个阶段，直接连接输入输出
                io.out = io.in;
            }
            else {
                // 否则，通过 wires 连接
                // 最后一个阶段的输出连接到输出端口
                io.out = wires.back();

                // 将 wires 的元素向后移动一个位置
                std::move_backward(wires.begin(), wires.end() - 1, wires.end());

                // 第一个阶段的输入连接到输入端口
                wires.front() = io.in;
            }
        }
    };
}