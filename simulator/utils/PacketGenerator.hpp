#pragma once

#include "Random.hpp"

namespace fpga::utils {

    // 一个抽象类，用于生成数据包，当 Input 未能从文件中读取相应数据时，可以使用这个类生成数据包
    // T: 数据包的类型
    template <typename T>
    struct PacketGenerator {
        virtual void operator()(std::vector<std::shared_ptr<T>>& vec) = 0;
    };

}