#pragma once

#include "../../utils.hpp"
#include "../../p4/Phv.hpp"

namespace fpga::basic {

    // 用于进行可编程流水线基本功能的测试
    template<decltype(auto) profile>
    struct BasicGenerator : public PacketGenerator<fpga::p4::Phv> {
        static constexpr size_t default_packet_num = 1000;

        static constexpr size_t count8 = profile.field8_count;
        static constexpr size_t count16 = profile.field16_count;
        static constexpr size_t count32 = profile.field32_count;

        // 生成随机数据包
        void operator()(std::vector<std::shared_ptr<fpga::p4::Phv>>& vec) override {
            for (size_t i = 0; i < default_packet_num; i++) {
                auto packet{ std::make_shared<fpga::p4::Phv>(i, 1) };
                for (size_t j = 0; j < count8; j++) {
                    packet->field8[j] = Random::get(0xFF);
                }
                for (size_t j = 0; j < count16; j++) {
                    packet->field16[j] = Random::get(0xFFFF);
                }
                for (size_t j = 0; j < count32; j++) {
                    packet->field32[j] = Random::get(0xFFFFFFFFULL);
                }
                vec.push_back(packet);
            }
        }

    };
}