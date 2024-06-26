#pragma once

#include "../../../utils.hpp"
#include "../../FieldBundleProfileOf.hpp"

namespace fpga::p4::mat::hash {

    template <size_t mau_id>
    struct HashUnitConfig {

    private:
        static constexpr size_t hash_count = fpga::Config::mat::hash::count;
        static constexpr size_t key_width = LookupKey::width;
        static constexpr size_t unit_size = fpga::Config::mat::hash::unit_size;
        static constexpr size_t value_width = fpga::Config::mat::hash::value::width;
        
    public:
        std::array<UInt<key_width>, hash_count> masks;
        std::array<UInt<value_width>, hash_count> salts;
        std::array<std::array<UInt<key_width>, value_width>, hash_count> matrices;

    private:

        // 编译期初始化
        template <size_t I, size_t J> requires (0 <= J && J < value_width)
            void init_matrix() {
            auto unit = Hex::from<&Program<mau_id>::mat::hash::matrix[I * value_width + J], unit_size>();
            matrices[I][J] = 0;
            for (size_t k = 0; k < key_width / unit_size; k++) {
                matrices[I][J] = (matrices[I][J] << unit_size) | unit;
            }
            if constexpr (J + 1 < value_width) {
                init_matrix<I, J + 1>();
            }
        }

        template <size_t I> requires (0 <= I && I < hash_count)
            void init() {
            masks[I] = Hex::from<&Program<mau_id>::mat::hash::mask[I], key_width>();
            salts[I] = Hex::from<&Program<mau_id>::mat::hash::salt[I], value_width>();
            init_matrix<I, 0>();
            if constexpr (I + 1 < hash_count) {
                init<I + 1>();
            }
        }

        // 单例模式，读取配置
        HashUnitConfig() {
            init<0>();
        }

    public:

        // 获取单例
        static constexpr HashUnitConfig& get() {
            static HashUnitConfig config;
            return config;
        }

    };

}