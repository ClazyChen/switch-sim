#pragma once

#include "../utils.hpp"
#include "Phv.hpp"

namespace fpga::p4 {

    // 用于流水线的模拟
    // 一个输入输出 Phv 的接口
    struct Pipe {
        In<Phv> phv_in;
        Out<Phv> phv_out;

        Pipe() = default;

        // 重置输出
        void reset() {
            phv_out = nullptr;
        }

        // 用于将 in 通过一级锁存传递到 out
        void pass() {
            phv_out = phv_in;
        }

        // 用于将外部流水线的 in 传递到内部流水线接口的 in
        void input_to(Pipe& other) {
            other.phv_in = phv_in;
        }

        // 用于将内部流水线的 out 传递到外部流水线接口的 out
        void output_to(Pipe& other) {
            phv_out = other.phv_out;
        }

        // 用于将同级流水线的 out 传递到同级流水线接口的 in
        void transport_to(Pipe& other) {
            other.phv_in = phv_out;
        }
    };
}