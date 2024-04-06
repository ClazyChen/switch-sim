#pragma once

#include "../utils.hpp"
#include "../p4/Phv.hpp"

namespace fpga::basic {

    // 用于进行可编程流水线基本功能的测试
    template <decltype(auto) profile>
    struct BasicAnalyzer : public InputAnalyzer<fpga::p4::Phv> {

        static constexpr size_t count8 = profile.field8_count;
        static constexpr size_t count16 = profile.field16_count;
        static constexpr size_t count32 = profile.field32_count;

        // input_file 格式:
        // [payload_cycles] [arrive_time] [field8] [filed16] [field32]

        void read(std::vector<std::shared_ptr<fpga::p4::Phv>>& vec, std::ifstream& ifs) override {
            size_t payload_cycles = 0;
            size_t arrive_time = 0;
            size_t field_temp = 0;
            vec.clear();
            while (ifs >> payload_cycles >> arrive_time) {
                auto packet = std::make_shared<fpga::p4::Phv>(arrive_time, payload_cycles);
                for (size_t i = 0; i < count8; i++) {
                    ifs >> field_temp;
                    packet->field8[i] = field_temp;
                }
                for (size_t i = 0; i < count16; i++) {
                    ifs >> field_temp;
                    packet->field16[i] = field_temp;
                }
                for (size_t i = 0; i < count32; i++) {
                    ifs >> field_temp;
                    packet->field32[i] = field_temp;
                }
                vec.push_back(packet);
            }
        }

        void write(const std::vector<std::shared_ptr<fpga::p4::Phv>>& vec, std::ofstream& ofs) override {
            for (const auto& packet : vec) {
                ofs << packet->payload_cycles << " " << packet->arrive_time << " ";
                for (size_t i = 0; i < count8; i++) {
                    ofs << packet->field8[i] << " ";
                }
                for (size_t i = 0; i < count16; i++) {
                    ofs << packet->field16[i] << " ";
                }
                for (size_t i = 0; i < count32; i++) {
                    ofs << packet->field32[i] << " ";
                }
                ofs << std::endl;
            }
        }

    };

}