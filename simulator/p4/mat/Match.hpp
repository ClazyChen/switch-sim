#pragma once

#include "getkey/GetKey.hpp"
#include "getkey/GetKeyConfig.hpp"

namespace fpga::p4::mat {

    template <size_t mau_id>
    struct Match : public Module {
        struct IO {
            Pipe pipe;
        } io;

        // 每一级流水线
        getkey::Getkey<mau_id> getkey;

        void reset() override {
            io.pipe.reset();
            getkey.reset();
        }

        void update() override {
            // 传入每一级流水线的 input
            io.pipe.input_to(getkey.io.pipe);

            // 运行每一级流水线的 update
            getkey.update();
        }
        
        void run() override {
            // 运行每一级流水线的 run
            getkey.run();
            
            // 将最后一级流水线的输出传递给出去
            getkey.io.pipe.output_to(io.pipe);
        }

    };
}