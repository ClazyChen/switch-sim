#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::transform {

    // 在表中没有查到时的默认值
    template <size_t mau_id>
    struct TransformConfig {
        static constexpr size_t value_count = fpga::Config::mat::transform::value_count;
        static constexpr size_t value_width = fpga::Config::mat::transform::value_width;

        std::array<UInt<value_width>, value_count> _default_value;

        template <size_t I> requires (I < value_count)
            constexpr auto init() {
            _default_value[I] = Hex::from<
                &Program<mau_id>::mat::transform::default_value[I],
                value_width
            >();
            if constexpr (I + 1 < value_count) {
                init<I + 1>();
            }
        }

        TransformConfig() {
            init<0>();
        }

    public:
        static constexpr const auto& default_value(size_t index) {
            static TransformConfig config;
            return config._default_value[index];
        }

    };
}