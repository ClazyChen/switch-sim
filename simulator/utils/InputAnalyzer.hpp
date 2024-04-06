#pragma once

#include "Random.hpp"
#include <fstream>

namespace fpga::utils {

    // 一个抽象类，用于将数据包输入和输出 input_file
    // T: 数据包的类型
    template <typename T>
    struct InputAnalyzer {
        virtual void read(std::vector<std::shared_ptr<T>>& vec, std::ifstream& ifs) = 0;
        virtual void write(const std::vector<std::shared_ptr<T>>& vec, std::ofstream& ofs) = 0;
    };
}