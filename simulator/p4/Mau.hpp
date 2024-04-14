#pragma once

#include "mat/Match.hpp"
#include "mem/SramCluster.hpp"

namespace fpga::p4 {

    // 用于实现一个完整的 MAU
    template <size_t mau_id>
    struct Mau : public Module {
    private:
        static constexpr size_t read_count = fpga::Config::mat::getaddress::read_count;
    public:
        struct IO {
            Pipe pipe;
        } io;

        // 每一级流水线
        mat::Match<mau_id> match;

        // 存储器
        mem::SramCluster sram;

        void reset() override {
            io.pipe.reset();
            match.reset();
            sram.reset();
        }

        void update() override {
            // 传入每一级流水线的 input
            io.pipe >> match.io.pipe;

            // 运行每一级流水线的 update
            match.update();

            // 将流水线中的访存请求发送给存储器
            //std::copy(match.io.match_read.begin(), match.io.match_read.end(), sram.io.match_read.begin());
            for (int i = 0; i < read_count; i++) {
                sram.io.match_read[i].en.get() = match.io.match_read[i].en.get();
                sram.io.match_read[i].sram_id.get() = match.io.match_read[i].sram_id.get();
                sram.io.match_read[i].addr.get() = match.io.match_read[i].addr.get();
                match.io.match_read[i].data.get() = sram.io.match_read[i].data.get();

            }
            sram.update();
        }

        void run() override {
            // 运行每一级流水线的 run
            match.run();

            // 运行存储器
            sram.run();

            // 将最后一级流水线的输出传递给出去
            io.pipe << match.io.pipe;
        }

    };

}