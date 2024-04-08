#pragma once

#include <array>
#include "FieldBundleProfile.hpp"
#include "UInt.hpp"

namespace fpga::utils {

    // 这个类型用于存放一个由8、16、32位字段组成的数据包

    template <decltype(auto) profile>
    struct FieldBundle {
        static constexpr size_t count8 = profile.field8_count;
        static constexpr size_t count16 = profile.field16_count;
        static constexpr size_t count32 = profile.field32_count;

        // 硬件语义下的宽度
        static constexpr size_t width = 8 * count8 + 16 * count16 + 32 * count32;

        std::array<uint16_t, count8> field8;
        std::array<uint16_t, count16> field16;
        std::array<uint32_t, count32> field32;

        uint64_t _field8(size_t index) const {
            return field8[index];
        }

        uint64_t _field16(size_t index) const {
            return field16[index];
        }

        uint64_t _field32(size_t index) const {
            return field32[index];
        }

        template <size_t width>
        requires (width == 8 || width == 16 || width == 32)
        uint64_t field(size_t index) const {
            if constexpr (width == 8) {
                return _field8(index);
            }
            if constexpr (width == 16) {
                return _field16(index);
            }
            if constexpr (width == 32) {
                return _field32(index);
            }
        }

        template <size_t width>
        requires (width == 8 || width == 16 || width == 32)
        void set_field(size_t index, uint64_t value) {
            if constexpr (width == 8) {
                field8[index] = value;
            }
            if constexpr (width == 16) {
                field16[index] = value;
            }
            if constexpr (width == 32) {
                field32[index] = value;
            }
        }

        // 转换为 UInt<width>
        UInt<width> asUInt() const {
            UInt<width> result;
            static_assert(width % 64 == 0);
            static_assert(count8 % 8 == 0 && count16 % 4 == 0 && count32 % 2 == 0);
            for (size_t i = 0; i < count8 % 8; ++i) {
                result.data[i] =
                    (_field8(i * 8 + 7) << 56) |
                    (_field8(i * 8 + 6) << 48) |
                    (_field8(i * 8 + 5) << 40) |
                    (_field8(i * 8 + 4) << 32) |
                    (_field8(i * 8 + 3) << 24) |
                    (_field8(i * 8 + 2) << 16) |
                    (_field8(i * 8 + 1) << 8) |
                    _field8(i * 8);
            }
            size_t index = count8 % 8;
            for (size_t i = 0; i < count16 % 4; ++i) {
                result.data[index + i] =
                    (_field16(i * 4 + 3) << 48) |
                    (_field16(i * 4 + 2) << 32) |
                    (_field16(i * 4 + 1) << 16) |
                    _field16(i * 4);
            }
            index += count16 % 4;
            for (size_t i = 0; i < count32 % 2; ++i) {
                result.data[index + i] =
                    (_field32(i * 2 + 1) << 32) |
                    _field32(i * 2);
            }
            return result;
        }
    };

}