#pragma once

#include "Wire.hpp"

namespace fpga::utils {

    // 用于流水线的模拟
    // 一个输入输出 信号 的接口
    template <typename T>
    struct Piped {
        In<T> in;
        Out<T> out;

        Piped() = default;

        // 重置输出
        void reset() {
            if constexpr (is_direct_type<T>) {
                out = 0;
            }
            else {
                out = nullptr;
            }
        }

        // 用于将 in 通过一级锁存传递到 out
        void pass() {
            out = in;
        }

        // 用于将外部流水线的 in 传递到内部流水线接口的 in
        Piped& operator>>(Piped& other) {
            other.in = in;
            return other;
        }

        // 用于将内部流水线的 out 传递到外部流水线接口的 out
        Piped& operator<<(Piped& other) {
            out = other.out;
            return other;
        }

        // 用于将同级流水线的 out 传递到同级流水线接口的 in
        Piped& operator>(Piped& other) {
            other.in = out;
            return other;
        }
    };

}