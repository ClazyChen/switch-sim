#pragma once
#include <iostream>
#include <climits>
#include <sstream>
#include "UInt.hpp"

namespace fpga::utils {

    // 将十六进制字符串转换为无符号整数
    struct Hex {

        // TODO: 非常丑陋的语法，有没有更好的方法？
        template <size_t w>
        static constexpr auto from(const char* const* str) {
            constexpr size_t n = w / 4; // 一个十六进制字符占 4 位
            UInt<w> result{};
            std::cout << *str << " " << w << std::endl;
            for (size_t i = 0; i < UInt<w>::n; i++) {
                std::string str_ = std::string(*str);
                std::string hex_string = str_.substr(16 * i, 16);
                std::stringstream ss(hex_string);
                ss >> std::hex >> result.data[i];
            }
            return result;
        }

    };

}