#pragma once

#include "SramClusterReadWrite.hpp"

namespace fpga::p4::mem {

    struct SramCluster : public Module {

    private:
        static constexpr size_t match_read_count = fpga::Config::mat::getaddress::read_count;
        static constexpr size_t salu_read_count = fpga::Config::act::executer::salu::all_count;
        static constexpr size_t salu_write_count = fpga::Config::act::executer::salu::all_count;
        static constexpr size_t ctrl_write_count = fpga::Config::memory::sram::ctrl_write_count;
        static constexpr size_t sram_count = fpga::Config::memory::sram::count;
        static constexpr size_t sram_id_width = fpga::Config::mat::getaddress::sram_id_width;
        static constexpr size_t sram_addr_width = fpga::Config::memory::sram::addr_width;
        static constexpr size_t sram_data_width = fpga::Config::memory::sram::data_width;

    public:
        struct IO {
            // 所有的读连接线
            std::array<SramClusterRead, match_read_count> match_read;
            std::array<SramClusterRead, salu_read_count> salu_read;

            // 所有的写连接线
            std::array<SramClusterWrite, salu_write_count> salu_write;
            std::array<SramClusterWrite, ctrl_write_count> ctrl_write;
        } io;

        std::vector<SramClusterRead*> reads;
        std::vector<SramClusterWrite*> writes;
        std::array<Sram<sram_addr_width, sram_data_width>, sram_count> srams;
        std::array<Wire<UInt<sram_addr_width>>, sram_count> sram_read_addrs;
        std::array<Wire<UInt<sram_addr_width>>, sram_count> sram_write_addrs;
        std::array<Wire<UInt<sram_data_width>>, sram_count> sram_write_datas;

        SramCluster() {
            std::transform(io.match_read.begin(), io.match_read.end(), std::back_inserter(reads), [](auto& r) { return &r; });
            std::transform(io.salu_read.begin(), io.salu_read.end(), std::back_inserter(reads), [](auto& r) { return &r; });
            std::transform(io.salu_write.begin(), io.salu_write.end(), std::back_inserter(writes), [](auto& w) { return &w; });
            std::transform(io.ctrl_write.begin(), io.ctrl_write.end(), std::back_inserter(writes), [](auto& w) { return &w; });
        }

        void reset() {
            for (size_t i = 0; i < sram_count; i++) {
                srams[i].reset();
                sram_read_addrs[i] = 0;
                sram_write_addrs[i] = 0;
                sram_write_datas[i] = 0;
            }
            for (auto read : reads) {
                read->data = 0;
            }
        }

        void update() {
            // 将所有的读写连接线的数据连到对应的SRAM上
            for (size_t i = 0; i < sram_count; i++) {
                srams[i].io.r.en = false;
                sram_read_addrs[i] = 0;
                srams[i].io.w.en = false;
                sram_write_addrs[i] = 0;
                sram_write_datas[i] = 0;
            }

            for (size_t i = 0; i < reads.size(); i++) {
                auto& read = *reads[i];
                if (read.sram_id.get().value() == i) {
                    srams[i].io.r.en = true;
                    sram_read_addrs[i].get() |= read.addr.get();
                }
            }

            for (size_t i = 0; i < writes.size(); i++) {
                auto& write = *writes[i];
                if (write.sram_id.get().value() == i) {
                    srams[i].io.w.en = true;
                    sram_write_addrs[i].get() |= write.addr.get();
                    sram_write_datas[i].get() |= write.data.get();
                }
            }

            for (size_t i = 0; i < sram_count; i++) {
                srams[i].io.r.addr = sram_read_addrs[i];
                srams[i].io.w.addr = sram_write_addrs[i];
                srams[i].io.w.data = sram_write_datas[i];
            }

            // 更新所有的SRAM（执行写）
            for (size_t i = 0; i < sram_count; i++) {
                srams[i].update();
            }
        }

        void run() {
            // 运行所有的SRAM（执行读）
            for (size_t i = 0; i < sram_count; i++) {
                srams[i].run();
            }

            // 将SRAM的数据传递给读连接线
            for (size_t i = 0; i < reads.size(); i++) {
                auto& read = *reads[i];
                read.data = srams[read.sram_id.get().value()].io.r.data;
            }
        }

    };

}