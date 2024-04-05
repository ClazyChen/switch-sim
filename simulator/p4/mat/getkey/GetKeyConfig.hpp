#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::getkey {

    template <size_t mau_id>
    struct GetKeyConfig {

        // KEY_LUT 的字段位置
        struct Lookup {

            static constexpr size_t index8(size_t index) {
                return Program<mau_id>::mat::getkey::lut::field8[index];
            }

            static constexpr size_t index16(size_t index) {
                return Program<mau_id>::mat::getkey::lut::field16[index];
            }

            static constexpr size_t index32(size_t index) {
                return Program<mau_id>::mat::getkey::lut::field32[index];
            }
        };

        // KEY_GTW 的字段位置
        struct Gateway {

            static constexpr size_t index8(size_t index) {
                return Program<mau_id>::mat::getkey::gtw::field8[index];
            }

            static constexpr size_t index16(size_t index) {
                return Program<mau_id>::mat::getkey::gtw::field16[index];
            }

            static constexpr size_t index32(size_t index) {
                return Program<mau_id>::mat::getkey::gtw::field32[index];
            }
        };

    };

}