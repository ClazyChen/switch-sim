#pragma once

#include "Module.hpp"
#include "Wire.hpp"
#include <fstream>

namespace fpga::utils {

    // 接收数据包的输出模块
    // T: 数据包类型
    template <typename T>
    struct Output : public Module {

        struct IO {
            In<T> packet_in;
        } io;

        // 用于输出统计结果信息
        std::ofstream fout;
        void initialize(const char* file_name) {
            fout.open(file_name);
        }

        int packet_count;

        ~Output() {
            fout.close();
        }

        void reset() override {
            packet_count = 0;
        }

        void update() override {
            if (io.packet_in) {
                packet_count++;
            }
        }

        void run() override {}

        virtual void report() {
            // 用于输出统计结果信息
        }
    };
}