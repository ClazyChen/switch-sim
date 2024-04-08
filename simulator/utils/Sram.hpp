#pragma once

#include "Wire.hpp"
#include "UInt.hpp"
#include "Module.hpp"

namespace fpga::utils {

    template <size_t addr_width, size_t data_width> 
    struct RPort {
        In<bool> en;
        In<UInt<addr_width>> addr;
        Out<UInt<data_width>> data;
    };

    template <size_t addr_width, size_t data_width>
    struct WPort {
        In<bool> en;
        In<UInt<addr_width>> addr;
        In<UInt<data_width>> data;
    };

    template <size_t addr_width, size_t data_width>
    struct Sram : public Module {

        struct IO {
            RPort<addr_width, data_width> r;
            WPort<addr_width, data_width> w;
        } io;

        std::array<UInt<data_width>, 1 << addr_width> mem;

        void reset() override {
            // 不重置 mem 的内容，保留默认值
            io.r.data = 0;
        }

        void update() override {
            // 只处理写
            if (io.w.en) {
                mem[io.w.addr.get().value()] = io.w.data.get();
            }
        }

        // 访问同一个地址时，由于写操作在 update 中处理，读操作在 run 中处理，所以读到的数据是上一次写入的数据
        void run() override {
            // 只处理读
            if (io.r.en) {
                io.r.data = mem[io.r.addr.get().value()];
            }
            else {
                io.r.data = 0;
            }
        }

    };

}