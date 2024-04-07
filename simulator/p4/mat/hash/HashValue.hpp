#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::hash {

    struct HashValue : public UInt<
        fpga::Config::mat::hash::value::width
    > {
    private:
        static constexpr size_t width = fpga::Config::mat::hash::value::width;
        static constexpr size_t chip_select_width = fpga::Config::mat::hash::value::chip_select_width;
        static constexpr bool kv_shared = fpga::Config::mat::hash::value::kv_shared;
        static constexpr size_t kv_pair_count = fpga::Config::mat::hash::value::kv_pair_count;
        static constexpr size_t kv_addr_width = fpga::Config::mat::hash::value::kv_addr_width;

    public:
        HashValue() : UInt<width>() {}
        HashValue(const UInt<width>& value) : UInt<width>(value) {}
        HashValue(UInt<width>&& value) : UInt<width>(std::move(value)) {}
        HashValue& operator=(const UInt<width>& value) {
            UInt<width>::operator=(value);
            return *this;
        }
        HashValue& operator=(UInt<width>&& value) {
            UInt<width>::operator=(std::move(value));
            return *this;
        }

        constexpr UInt<chip_select_width> chip_select() const {
            return slice<width - 1, width - chip_select_width>();
        }

        template <size_t index>
        constexpr UInt<kv_addr_width> key() const {
            if constexpr (kv_shared) {
                return slice<(index + 1) * kv_addr_width - 1, index * kv_addr_width>(); 
            }
            else {
                return slice<(index * 2 + 1) * kv_addr_width - 1, (index * 2) * kv_addr_width>();
            }
        }

        template <size_t index>
        constexpr UInt<kv_addr_width> value() const {
            if constexpr (kv_shared) {
                return key<index>();
            }
            else {
                return slice<(index * 2 + 2) * kv_addr_width - 1, (index * 2 + 1) * kv_addr_width>();
            }
        
        }
    };

}