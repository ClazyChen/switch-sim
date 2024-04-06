#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <format>

namespace fpga::utils {

    // 用于存储无符号大整数的类，对应 Chisel 中的 UInt
    template <size_t w>
        requires (w > 0)
    struct UInt {
        static constexpr size_t width = w;
        static constexpr size_t n = (w + 63) / 64;

        // 标识 data[-1] 的有效位掩码
        static constexpr uint64_t tail_mask = (1ULL << (w % 64)) - 1;

        // 设置或清除 data[-1] 的无效位
        constexpr UInt& set_invalid_bits() {
            if constexpr (w % 64 != 0) {
                data[n - 1] |= ~tail_mask;
            }
            return *this;
        }
        constexpr UInt& clear_invalid_bits() {
            if constexpr (w % 64 != 0) {
                data[n - 1] &= tail_mask;
            }
            return *this;
        }

        // 用于存储大整数的数组
        // data[0] 存储最低位，data[n - 1] 存储最高位
        std::array<uint64_t, n> data;

        constexpr UInt() = default;

        template <size_t u>
        constexpr UInt(const UInt<u>& other) {
            if constexpr (UInt<u>::n < UInt<w>::n) {
                std::copy(other.data.begin(), other.data.end(), data.begin());
                std::fill(data.begin() + UInt<u>::n, data.end(), 0);
            }
            else {
                std::copy(other.data.begin(), other.data.begin() + UInt<w>::n, data.begin());
            }
        }

        template <size_t u>
        constexpr UInt(UInt<u>&& other) {
            if constexpr (UInt<u>::n < UInt<w>::n) {
                std::move(other.data.begin(), other.data.end(), data.begin());
                std::fill(data.begin() + UInt<u>::n, data.end(), 0);
            }
            else {
                std::move(other.begin(), other.begin() + UInt<w>::n, data.begin());
            }
        }

        template <size_t u>
        constexpr UInt& operator=(const UInt<u>& other) {
            if constexpr (UInt<u>::n < UInt<w>::n) {
                std::copy(other.data.begin(), other.data.end(), data.begin());
                std::fill(data.begin() + UInt<u>::n, data.end(), 0);
            }
            else {
                std::copy(other.data.begin(), other.data.begin() + UInt<w>::n, data.begin());
            }
            return *this;
        }

        template <size_t u>
        constexpr UInt& operator=(UInt<u>&& other) {
            if constexpr (UInt<u>::n < UInt<w>::n) {
                std::move(other.data.begin(), other.data.end(), data.begin());
                std::fill(data.begin() + UInt<u>::n, data.end(), 0);
            }
            else {
                std::move(other.data.begin(), other.data.begin() + UInt<w>::n, data.begin());
            }
            return *this;
        }

        constexpr UInt(uint64_t value) {
            data[0] = value;
            std::fill(data.begin() + 1, data.end(), 0);
        }

        constexpr UInt& operator=(uint64_t value) {
            data[0] = value;
            std::fill(data.begin() + 1, data.end(), 0);
            return *this;
        }

        // 定义所有运算符
        template <size_t u>
        constexpr UInt<std::max(u, w)> operator|(const UInt<u>& other) const {
            UInt<std::max(u, w)> result;
            if constexpr (UInt<u>::n < UInt<w>::n) {
                std::transform(other.data.begin(), other.data.end(), data.begin(), result.begin(), std::bit_or<uint64_t>());
                std::copy(data.begin() + UInt<u>::n, data.end(), result.data.begin() + UInt<u>::n);
            }
            else {
                std::transform(data.begin(), data.end(), other.data.begin(), result.begin(), std::bit_or<uint64_t>());
                std::copy(other.data.begin() + UInt<w>::n, other.data.end(), result.data.begin() + UInt<w>::n);
            }
            return result;
        }

        template <size_t u>
        constexpr UInt<std::max(u, w)> operator&(const UInt<u>& other) const {
            UInt<std::max(u, w)> result;
            // 因为 0 & 1 = 0，所以这里需要先转换成前缀 1 的形式
            set_invalid_bits();
            other.set_invalid_bits();
            if constexpr (UInt<u>::n < UInt<w>::n) {
                std::transform(other.data.begin(), other.data.end(), data.begin(), result.begin(), std::bit_and<uint64_t>());
                std::fill(result.data.begin() + UInt<u>::n, result.data.end(), 0);
            }
            else {
                std::transform(data.begin(), data.end(), other.data.begin(), result.begin(), std::bit_and<uint64_t>());
                std::fill(result.data.begin() + UInt<w>::n, result.data.end(), 0);
            }
            // 计算完成之后再将无效位清除
            clear_invalid_bits();
            other.clear_invalid_bits();
            return result;
        }

        template <size_t u>
        constexpr UInt<std::max(u, w)> operator^(const UInt<u>& other) const {
            UInt<std::max(u, w)> result;
            if constexpr (UInt<u>::n < UInt<w>::n) {
                std::transform(other.data.begin(), other.data.end(), data.begin(), result.begin(), std::bit_xor<uint64_t>());
                std::copy(data.begin() + UInt<u>::n, data.end(), result.data.begin() + UInt<u>::n);
            }
            else {
                std::transform(data.begin(), data.end(), other.data.begin(), result.begin(), std::bit_xor<uint64_t>());
                std::copy(other.data.begin() + UInt<w>::n, other.data.end(), result.data.begin() + UInt<w>::n);
            }
            return result;
        }

        constexpr UInt<w> operator~() const {
            UInt<w> result;
            std::transform(data.begin(), data.end(), result.data.begin(), std::bit_not<uint64_t>());
            result.clear_invalid_bits();
            return result;
        }

        constexpr UInt<w> operator<<(size_t u) const {
            UInt<w> result;
            size_t shift = u / 64;
            size_t offset = u % 64;
            for (size_t i = 0; i < n; i++) {
                if (i + shift + 1 < n) {
                    result.data[i + shift + 1] |= (data[i] >> (64 - offset));
                }
                if (i + shift < n) {
                    result.data[i + shift] |= (data[i] << offset);
                }
            }
            result.clear_invalid_bits();
            return result;
        }

        constexpr UInt<w> operator>>(size_t u) const {
            UInt<w> result;
            size_t shift = u / 64;
            size_t offset = u % 64;
            for (size_t i = 0; i < n; i++) {
                if (i >= shift + 1) {
                    result.data[i - shift - 1] |= (data[i] << (64 - offset));
                }
                if (i >= shift) {
                    result.data[i - shift] |= (data[i] >> offset);
                }
            }
            result.clear_invalid_bits();
            return result;
        }

        template <size_t u>
        friend constexpr bool operator==(UInt<w> lhs, UInt<u> rhs) {
            if constexpr (UInt<u>::n < UInt<w>::n) {
                return std::equal(lhs.data.begin(), lhs.data.begin() + UInt<u>::n, rhs.data.begin());
            }
            else {
                return std::equal(lhs.data.begin(), lhs.data.end(), rhs.data.begin());
            }
        }

        template <size_t u>
        friend constexpr auto operator<=>(UInt<w> lhs, UInt<u> rhs) {
            if constexpr (UInt<u>::n < UInt<w>::n) {
                UInt<w> temp = rhs;
                return std::lexicographical_compare_three_way(lhs.data.rbegin(), lhs.data.rend(), temp.data.rbegin(), temp.data.rend());
            }
            else {
                UInt<u> temp = lhs;
                return std::lexicographical_compare_three_way(temp.data.rbegin(), temp.data.rend(), rhs.data.rbegin(), rhs.data.rend());
            }
        }

        template <size_t u>
        friend constexpr UInt<std::max(u, w)> operator+(const UInt<w>& lhs, const UInt<u>& rhs) {
            UInt<std::max(u, w)> result;
            UInt<std::max(u, w)> lhs_extend = lhs;
            UInt<std::max(u, w)> rhs_extend = rhs;
            uint64_t carry = 0;
            for (size_t i = 0; i < std::max(UInt<u>::n, UInt<w>::n); i++) {
                uint64_t temp = lhs_extend.data[i] + rhs_extend.data[i] + carry;
                result.data[i] = temp;
                carry = temp < lhs_extend.data[i] || temp < rhs_extend.data[i];
            }
            return result.clear_invalid_bits();
        }

        friend constexpr UInt<w> operator+(const UInt<w>& lhs, uint64_t rhs) {
            UInt<w> result;
            result.data[0] = lhs.data[0] + rhs;
            uint64_t carry = result.data[0] < lhs.data[0];
            for (size_t i = 1; i < n; i++) {
                result.data[i] = lhs.data[i] + carry;
                carry = result.data[i] < lhs.data[i];
            }
            return result.clear_invalid_bits();
        }

        friend constexpr UInt<w> operator+(uint64_t lhs, const UInt<w>& rhs) {
            return rhs + lhs;
        }

        // 负号，补码表示
        constexpr UInt<w> operator-() const {
            return (~(*this) + 1).clear_invalid_bits();
        }

        template <size_t u>
        friend constexpr UInt<std::max(u, w)> operator-(const UInt<w>& lhs, const UInt<u>& rhs) {
            return lhs + (-rhs);
        }

        friend constexpr UInt<w> operator-(const UInt<w>& lhs, uint64_t rhs) {
            return lhs - UInt<64>(rhs);
        }

        friend constexpr UInt<w> operator-(uint64_t lhs, const UInt<w>& rhs) {
            return UInt<64>(lhs) - rhs;
        }

        // 赋值运算符
        template <size_t u>
        constexpr UInt<w>& operator|=(const UInt<u>& other) {
            return *this = *this | other;
        }

        template <size_t u>
        constexpr UInt<w>& operator&=(const UInt<u>& other) {
            return *this = *this & other;
        }

        template <size_t u>
        constexpr UInt<w>& operator^=(const UInt<u>& other) {
            return *this = *this ^ other;
        }

        constexpr UInt<w>& operator<<=(size_t u) {
            return *this = *this << u;
        }

        constexpr UInt<w>& operator>>=(size_t u) {
            return *this = *this >> u;
        }

        constexpr UInt<w>& operator+=(uint64_t value) {
            return *this = *this + value;
        }

        template <size_t u>
        constexpr UInt<w>& operator+=(const UInt<u>& other) {
            return *this = *this + other;
        }

        constexpr UInt<w>& operator-=(uint64_t value) {
            return *this = *this - value;
        }

        template <size_t u>
        constexpr UInt<w>& operator-=(const UInt<u>& other) {
            return *this = *this - other;
        }

        // 取单个比特
        constexpr uint64_t operator()(size_t index) const {
            return (data[index / 64] >> (index % 64)) & 1;
        }

        // 取 slice，编译期
        template <size_t hi, size_t lo>
            requires (hi >= lo) && (hi < w)
        constexpr UInt<hi - lo + 1> slice() const {
            return UInt<hi - lo + 1>(*this >> lo);
        }

        // 合并两个 UInt，使用运算符 && 实现
        template <size_t u>
        friend constexpr UInt<w + u> operator&&(const UInt<w>& lhs, const UInt<u>& rhs) {
            UInt<w + u> temp = lhs;
            return (temp << u) | rhs;
        }


    };

}

template <size_t w>
struct std::formatter<fpga::utils::UInt<w>> {

    // 支持的格式化选项：
    // - 'Bb'：二进制（大写表示包含b前缀）
    // - 'Oo'：八进制
    // - 'Hh'：十六进制
    // - 'Dd'：十进制（仅对不大于64位的整数有效）
    // - 'w'：显示位宽
    // - '0'：显示前导零（仅对二进制有效）

    char format_type = 'x';
    bool display_format = false;
    bool display_width = false;
    bool display_zero = false;

    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin(), end = ctx.end();

        while (it != end) {
            if (*it == 'B' || *it == 'b' || *it == 'O' || *it == 'o' || *it == 'H' || *it == 'h' || *it == 'D' || *it == 'd') {
                display_format = std::isupper(*it);
                format_type = std::tolower(*it);
            } else if (*it == 'w') {
                display_width = true;
            } else if (*it == '0') {
                display_zero = true;
            } else if (*it == '}') {
                break;
            }
            it++;
        }
        return it;
    }

private:

    static constexpr void remove_leading_zeros(std::string& str) {
        str = str.substr(str.find_first_not_of('0'));
        if (str.empty()) {
            str = "0";
        }
    }

public:
    template <typename FormatContext>
    auto format(const fpga::utils::UInt<w>& value, FormatContext& ctx) const {
        std::string result;
        if (display_width) {
            result += std::format("{}\'", w);
        }
        if (display_format) {
            result += format_type;
        }
        if (format_type == 'd') {
            if constexpr (w <= 64) {
                // 可以被正常输出为十进制数
                result += std::format("{}", value.data[0]);
            } else {
                // 位宽过大，无法输出
                result += "<too large>";
            }
        }
        else {
            if (format_type == 'b') {
                // 二进制
                std::string binary;
                binary.resize(w);
                for (size_t i = 0; i < w; i++) {
                    binary[w - i - 1] = '0' + value(i);
                }
                if (!display_zero) {
                    // 不显示前导 0
                    remove_leading_zeros(binary);
                }
                result += binary;
            }
            else if (format_type == 'o') {
                // 八进制
                std::string octal;
                octal.resize((w + 2) / 3);
                for (size_t i = 0; i < (w + 2) / 3; i++) {
                    uint64_t temp = 0;
                    for (size_t j = 0; j < 3 && i * 3 + j < w; j++) {
                        temp |= value(i * 3 + j) << j;
                    }
                    octal[(w + 2) / 3 - i - 1] = '0' + temp;
                }
                remove_leading_zeros(octal);
                result += octal;
            } else if (format_type == 'h') {
                // 十六进制
                std::string hex;
                hex.resize((w + 3) / 4);
                for (size_t i = 0; i < (w + 3) / 4; i++) {
                    uint64_t temp = 0;
                    for (size_t j = 0; j < 4 && i * 4 + j < w; j++) {
                        temp |= value(i * 4 + j) << j;
                    }
                    if (temp < 10) {
                        hex[(w + 3) / 4 - i - 1] = '0' + temp;
                    } else {
                        hex[(w + 3) / 4 - i - 1] = 'a' + temp - 10;
                    }
                }
                remove_leading_zeros(hex);
                result += hex;
            }
        }

    }

};

namespace fpga::utils {

    template <size_t w>
    std::ostream& operator<<(std::ostream& os, const UInt<w>& value) {
        return os << std::format("{:w}", value);
    }

}