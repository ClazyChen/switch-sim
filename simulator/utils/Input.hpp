#pragma once

#include "Module.hpp"
#include "Wire.hpp"
#include "PacketGenerator.hpp"
#include "InputAnalyzer.hpp"
#include <fstream>
#include <filesystem>

namespace fpga::utils {

    // 发送数据包的输入模块
    // T: 数据包的类型
    // G: 数据包生成器的类型
    // I: 数据包输入文件接口
    template <typename T, typename G, typename I> 
    requires (std::is_base_of_v<PacketGenerator<T>, G> && std::is_base_of_v<InputAnalyzer<T>, I>)
    struct Input : public Module {

        // 定义输出接口（这个模块只有一个输出接口，用于发送数据包）
        struct IO {
            Out<T> packet_out;
        } io;

        G generator; // 数据包生成器
        I input_anaylzer; // 数据包输入文件接口

        std::vector<std::shared_ptr<T>> packets;

        // 输入模块的实现依赖于下列抽象接口
        // 基于一个文件初始化输入模块，读入所需数据
        void initialize(const char* input_file) {
            if (!std::filesystem::exists(input_file)) {
                generator(packets);
                if (std::ofstream ofs(input_file); ofs.good()) {
                    input_anaylzer.write(packets, ofs);
                    ofs.close();
                }
                else {
                    throw std::runtime_error("Failed to open file for writing: " + std::string(input_file));
                }
            }
            else {
                if (std::ifstream ifs(input_file); ifs.good()) {
                    input_anaylzer.read(packets, ifs);
                    ifs.close();
                }
                else {
                    throw std::runtime_error("Failed to open file for reading: " + std::string(input_file));
                }
            }
        }

        uint64_t global_cycle = 0;
        size_t packet_index = 0;

        // 从输入模块中获取下一个数据包，如果当前周期没有数据包，则返回nullptr
        std::shared_ptr<T> next() {
            if (packet_index < packets.size() && global_cycle++ == packets[packet_index]->arrive_time) {
                return packets[packet_index++];
            }
            else {
                return nullptr;
            }
        }

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