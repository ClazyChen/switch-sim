#pragma once

namespace fpga::utils {

    // 编译期计算 Log2（向上取整）
    template <size_t N>
    struct Log2Ceil {
        static constexpr size_t value = 1 + Log2Ceil<(N + 1) / 2>::value;
    };

    template <>
    struct Log2Ceil<1> {
        static constexpr size_t value = 0;
    };

}