#pragma once

#include "mat/Match.hpp"
#include "mem/SramCluster.hpp"
#include "../Config.hpp"
#include <iostream>

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

            for (int i = 0; i < read_count; i++) {
                sram.io.match_read[i].en = match.io.match_read[i].en.out;
                sram.io.match_read[i].sram_id = match.io.match_read[i].sram_id.out;
                sram.io.match_read[i].addr = match.io.match_read[i].addr.out;
                std::cout << sram.io.match_read[i].en << " ";
            }
            std::cout << std::endl;

            sram.update();

            // 将流水线中的访存请求发送给存储器
           // std::copy(match.io.match_read.begin(), match.io.match_read.end(), sram.io.match_read.begin());
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