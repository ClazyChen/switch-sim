#pragma once

#include <iostream>
#include <map>

namespace fpga::test {

    // 用于使用不同的颜色输出
    enum class Color {
        Black, Red, Green, Yellow, Blue, Purple, Cyan, White
    };

    struct Console {
    private:

        // 颜色和对应的代码
        static inline const std::map<Color, char> color_code = {
            { Color::Black, '0'},
            { Color::Red, '1'},
            { Color::Green, '2'},
            { Color::Yellow, '3'},
            { Color::Blue, '4'},
            { Color::Purple, '5'},
            { Color::Cyan, '6'},
            { Color::White, '7'}
        };

    public:

        // 使用给定的颜色输出
        template <Color color = Color::Black>
        static void log(const auto& arg) {
            std::cout << "\033[1;" << color_code.at(color) << "m" << arg << "\033[0m" << std::endl;
        }

    };


}