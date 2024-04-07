#pragma once

#include "MatPacket.hpp"
#include "../utils/Wire.hpp"
#include "../utils/Module.hpp"
#include "../program/Program.hpp"
#include <vector>
#include <fstream>
#include <array>

namespace fpga::mat {
    template<
        size_t FIELD8_NUM,
        size_t FIELD16_NUM,
        size_t FIELD32_NUM,
        typename PROFILE
        //typename PROFILE
    >
    struct MatInput : public fpga::utils::Module {

        //PROFILE profile;
        std::vector<std::shared_ptr<fpga::mat::MatPacket>> packets;
        struct IO {
            fpga::utils::Out<fpga::mat::MatPacket> packet_out;
        } io;

        void reset() override {
            /*Base::reset();
            global_clock = 0;*/
        }

        void initialize(const char* input_file) {
            // input_file 格式:
            // [payload_cycles] [arrive_time] [field8] [filed16] [field32]
            packets.clear();
            std::ifstream ifs(input_file);
            if (!ifs.is_open()) {
                throw std::runtime_error("MatInput: open input file failed: " + std::string(input_file));
            }
            /*int version, ihl, diffserv, flags, ttl, protocol, totalLen, fragOffset, hdrChecksum, payload_cycles;
            long long arrive_time, src_addr, dst_addr;*/
            int index = 0;
            int sum = FIELD8_NUM + FIELD16_NUM + FIELD32_NUM + 2;//一个数据包总共的字段个数
            std::array<int, FIELD8_NUM> field8_temp;
            std::array<int, FIELD16_NUM> field16_temp;
            std::array<int, FIELD32_NUM> field32_temp;
            /*std::array<int, 19> field8_;
            std::array<int, 18> field16_;
            std::array<int, 17> field32_;*/
            std::array<int, PROFILE::phv::count8> field8_;
            std::array<int, PROFILE::phv::count16> field16_;
            std::array<int, PROFILE::phv::count32> field32_;
            long long arrive_time = -1;
            int payload_cycles = -1;

            while (!ifs.eof()) {
                int remainder = index % sum;
                if (remainder == 0) {
                    if (index != 0) {
                        //要先把上次的数据保存

                        for (int i = 0; i < FIELD8_NUM; i++) {
                            field8_[i] = field8_temp[i];
                        }
                        for (int i = 0; i < FIELD16_NUM; i++) {
                            field16_[i] = field16_temp[i];
                        }
                        for (int i = 0; i < FIELD32_NUM; i++) {
                            field32_[i] = field32_temp[i];
                        }



                        packets.push_back(std::make_shared<MatPacket>(field8_, field16_, field32_, payload_cycles, arrive_time));
                        //产生一个新的数据包，并且调用构造函数






                    }
                    ifs >> payload_cycles;
                }
                else if (remainder == 1) { ifs >> arrive_time; }
                else if (remainder > 1 && remainder <= 1 + FIELD8_NUM) { ifs >> field8_temp[remainder - 2]; }
                else if (remainder > 1 + FIELD8_NUM && remainder <= 1 + FIELD8_NUM + FIELD16_NUM) { ifs >> field16_temp[remainder - 2 - FIELD8_NUM]; }
                else { ifs >> field32_temp[remainder - 2 - FIELD8_NUM - FIELD16_NUM]; }


                index++;
                /*ifs >> version >> ihl >> diffserv >> flags >> ttl >> protocol >> totalLen >> fragOffset >> hdrChecksum >> payload_cycles;
                ifs >> arrive_time >> src_addr >> dst_addr;
                packets.push_back(std::make_shared<AtpPacket>(worker_id, agg_id, non_atp, payload_cycles, arrive_time));*/

            }


            for (int i = 0; i < FIELD8_NUM; i++) {
                field8_[i] = field8_temp[i];
            }
            for (int i = 0; i < FIELD16_NUM; i++) {
                field16_[i] = field16_temp[i];
            }
            for (int i = 0; i < FIELD32_NUM; i++) {
                field32_[i] = field32_temp[i];
            }

            packets.push_back(std::make_shared<MatPacket>(field8_, field16_, field32_, payload_cycles, arrive_time));
            ifs.close();
            index = 0;
        }

        // 全局时钟
        long long global_clock = 0;
        int index = 0;



        std::shared_ptr<MatPacket> next() {
            if (index < packets.size() && global_clock++ == packets[index]->arrive_time) {
                auto packet{ packets[index++] };
                return packet;
            }
            return nullptr;
        }

        void update() override {
        }

        void run() override {
            // 向外部发送一个数据包
            if (auto packet{ next() }; packet) {
                io.packet_out = std::move(packet);
            }
            else {
                io.packet_out = nullptr;
            }
        }

    };
}