#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::gateway {

    template <size_t mau_id>
    struct GatewayTranslateConfig {

    private:
        std::unordered_map<size_t, size_t> gateway_hash_table;

        // 构造函数，采用单例模式，传入映射表
        template <size_t N>
        requires (N % 2 == 0)
        GatewayTranslateConfig(const std::array<int, N>& table) {
            for (size_t i = 0; i < N; i += 2) {
                gateway_hash_table[table[i]] = table[i + 1];
            }
        }

        // 获取单例
        static GatewayTranslateConfig& get_instance() {
            static GatewayTranslateConfig instance(Program<mau_id>::mat::gateway::translate);
            return instance;
        }

    public:

        // 通过哈希结果返回查表结果（网关 ID）
        // 如果无法找到则返回 0
        static size_t translate(size_t hash) {
            auto& inst = get_instance().gateway_hash_table;
            if (auto it{ inst.find(hash) }; it != inst.end()) {
                return it->second;
            }
            else {
                return 0;
            }
        }

    };

}