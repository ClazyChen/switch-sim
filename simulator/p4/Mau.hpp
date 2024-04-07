#pragma once

#include "mat/Match.hpp"

namespace fpga::p4 {

    // 用于实现一个完整的 MAU
    template <size_t mau_id>
    struct Mau : public Module {
        struct IO {
            Pipe pipe;
        } io;

        // 每一级流水线
        mat::Match<mau_id> match;

        void reset() override {
            io.pipe.reset();
            match.reset();
        }

        void update() override {
            // 传入每一级流水线的 input
            io.pipe >> match.io.pipe;

            // 运行每一级流水线的 update
            match.update();
        }

        void run() override {
            // 运行每一级流水线的 run
            match.run();

            // 将最后一级流水线的输出传递给出去
            io.pipe << match.io.pipe;
        }

    };

}