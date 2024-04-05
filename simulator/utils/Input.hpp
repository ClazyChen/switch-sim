#pragma once

#include "Module.hpp"
#include "Wire.hpp"

namespace fpga::utils {

    // 发送数据包的输入模块
    // T: 数据包的类型
    template <typename T> 
    struct Input : public Module {

        // 定义输出接口（这个模块只有一个输出接口，用于发送数据包）
        struct IO {
            Out<T> packet_out;
        } io;

        // 输入模块的实现依赖于下列抽象接口
        // 基于一个文件初始化输入模块，读入所需数据
        virtual void initialize(const char* input_file) = 0;

        // 从输入模块中获取下一个数据包，如果当前周期没有数据包，则返回nullptr
        virtual std::shared_ptr<T> next() = 0;

        void reset() override {
            io.packet_out = nullptr;
        }

        void update() override {}

        void run() override {
            // 向外部发送一个数据包
            io.packet_out = next();
        }
    };
}