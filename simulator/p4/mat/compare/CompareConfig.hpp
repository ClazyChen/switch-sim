#pragma once

#include "../../../utils.hpp"
#include <iostream>

namespace fpga::p4::mat::compare {

    // 用于实现比较模块
    template <size_t mau_id>
    struct CompareConfig {

        static constexpr size_t key_part_count = fpga::Config::mat::compare::key_part_start.size();
        static constexpr size_t mask_unit_count = fpga::Config::mat::compare::mask_unit;
        static constexpr size_t mask_unit_width = fpga::Config::mat::compare::mask_width;
        static constexpr size_t key_part_index_width = fpga::utils::Log2Ceil<key_part_count>::value;

        static constexpr auto key_part_index(size_t index) {
            return UInt<key_part_index_width>(Program<mau_id>::mat::compare::key_part_index[index]);
        }

        static constexpr auto mask(size_t index) {
            static constexpr size_t mask_width = mask_unit_count * mask_unit_width;
            auto mask_units = Program<mau_id>::mat::compare::mask[index];
            UInt<mask_width> mask = 0;
            UInt<mask_width> mask1 = (1 << mask_unit_width) - 1;
            for (size_t i = 0; i < mask_unit_count; i++) {
                if (mask_units & 1) {
                    mask |= (mask1 << (i * mask_unit_width));
                    
                }
                mask_units >>= 1;
            }
            return mask;
        }
    };
}