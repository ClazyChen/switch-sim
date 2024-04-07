#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::gateway {

    template <size_t mau_id>
    struct GatewayCompareConfig {

    private:
        // 获取对应 index 的 gateway 使用的 op
        template <size_t width>
        requires (width == 8 || width == 16 || width == 32)
        static constexpr const char* op(size_t index) {
            if constexpr (width == 8) {
                return Program<mau_id>::mat::gateway::gateway8_op[index];
            }
            if constexpr (width == 16) {
                return Program<mau_id>::mat::gateway::gateway16_op[index];
            }
            if constexpr (width == 32) {
                return Program<mau_id>::mat::gateway::gateway32_op[index];
            }
        }

        // 获取对应 index 的 gateway 使用的 field_id
        // 注意 gateway #index 所使用的 field_id 是 index * 2 和 index * 2 + 1
        template <size_t width>
        requires (width == 8 || width == 16 || width == 32)
        static constexpr size_t id(size_t index) {
            if constexpr (width == 8) {
                return Program<mau_id>::mat::gateway::gateway8[index];
            }
            if constexpr (width == 16) {
                return Program<mau_id>::mat::gateway::gateway16[index];
            }
            if constexpr (width == 32) {
                return Program<mau_id>::mat::gateway::gateway32[index];
            }
        }

    public:
        // 输入 gateway 的 index，返回 (field_index1, field_index2, gateway_op)
        template <size_t width>
        requires (width == 8 || width == 16 || width == 32)
        static constexpr std::tuple<size_t, size_t, const char*> config(size_t index) {
            if constexpr (fpga::Config::mat::gateway::any_connect) {
                // 任意连接，从 gateway 的配置中读取
                return std::make_tuple(
                    id<width>(index * 2),
                    id<width>(index * 2 + 1),
                    op<width>(index)
                );
            }
            else {
                // 直接连接
                return std::make_tuple(
                    index * 2,
                    index * 2 + 1,
                    op<width>(index)
                );
            }
        }

    };

}