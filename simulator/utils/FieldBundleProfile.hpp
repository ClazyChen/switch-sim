#pragma once

#include "../Config.hpp"

namespace fpga::utils {
    struct FieldBundleProfile {
        const size_t field8_count;
        const size_t field16_count;
        const size_t field32_count;

        constexpr size_t sum() const {
            return field8_count + field16_count + field32_count;
        }
    };
}