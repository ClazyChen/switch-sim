#pragma once

#include "../utils/Module.hpp"

namespace fpga::test {

    // 用于生成测试实例的模块（基类）
    struct TestInstance : public fpga::utils::Module {

        // 在原有的 reset 接口（重置）
        //         update 接口（组合逻辑）
        //         run 接口（时序逻辑）
        // 的基础上，增加了一个 init 接口（初始化）
        // 接受两个参数
        // 1. input_file: 输入文件名
        // 2. output_file: 输出文件名
        virtual void initialize(const char* input_file, const char* output_file) = 0;

        // 增加一个接口 用于在测试结构之后输出统计信息
        virtual void report() = 0;
    };
}