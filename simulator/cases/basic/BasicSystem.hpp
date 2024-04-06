#pragma once

#include "BasicAnalyzer.hpp"
#include "BasicGenerator.hpp"
#include "../../p4/Pipeline.hpp"
#include "../../test/TestInstance.hpp"

namespace fpga::cases::basic {

    // 用于进行可编程流水线基本功能的测试
    template <
          typename I     // 流水线的输入类型，需要继承 Input
        , typename O     // 流水线的输出类型，需要继承 Output
        , size_t N       // 流水线的阶段数量
    >
    struct BasicSystem : public fpga::test::TestInstance {

        I input;
        O output;

        // 主流水线
        fpga::p4::Pipeline<N> pipeline;

        void initialize(const char* input_file, const char* output_file) override {
            input.initialize(input_file);
            output.initialize(output_file);
        }

        void reset() override {
            input.reset();
            output.reset();
            pipeline.reset();
        }

        void update() override {
            // 输入模块（视为组合逻辑电路）
            input.update();

            // 从输入模块读取数据
            pipeline.io.pipe.phv_in = input.io.packet_out;

            // 流水线内部逻辑
            pipeline.update();

            // 向输出模块发射数据
            output.io.packet_in = pipeline.io.pipe.phv_out;

            // 输出模块（视为组合逻辑电路）
            output.update();
        }

        void run() override {
            input.run();
            pipeline.run();
            output.run();
        }

        void report() override {
            output.report();
        }

    };

}