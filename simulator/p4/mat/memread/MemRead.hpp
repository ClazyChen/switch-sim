#pragma once

#include "MemReadConfig.hpp"
#include "../../mem/SramCluster.hpp"
#include "../../Pipe.hpp"

namespace fpga::p4::mat::memread {

    template <size_t mau_id>
    struct MemRead : public Module {

    private:
        static constexpr size_t level_count = fpga::Config::mat::memread::level_count;
        static constexpr size_t gateway_width = fpga::Config::mat::gateway::width;
        static constexpr size_t read_count = fpga::Config::mat::getaddress::read_count;
        static constexpr size_t sram_id_width = fpga::Config::mat::getaddress::sram_id_width;
        static constexpr size_t sram_data_width = fpga::Config::mat::memread::sram_data_width;
        static constexpr size_t on_chip_addr_width = fpga::Config::mat::getaddress::on_chip_addr_width;

    public:
        
        struct IO {
            Pipe pipe;
            Piped<LookupKey> key;
            In<UInt<gateway_width>> gateway_in;
            std::array<In<UInt<sram_id_width>>, read_count> sram_id_in;
            std::array<In<UInt<on_chip_addr_width>>, read_count> on_chip_addr_in;
            std::array<Out<bool>, read_count> read_en_out;
            std::array<Out<UInt<sram_data_width>>, read_count> read_out;
            std::array<fpga::p4::mem::SramClusterRead, read_count> match_read;
        } io;

    private:
        // 缓存 read_en 和 read_out
        std::array<Wire<bool>, read_count> read_en;
        std::array<Wire<UInt<sram_data_width>>, read_count> read_data;

        Delay<Phv, level_count, In> delay_phv{ io.pipe };
        Delay<LookupKey, level_count, In> delay_key{ io.key };
        std::vector<std::unique_ptr<Delay<bool, level_count>>> delay_read_en;
        std::vector<std::unique_ptr<Delay<UInt<sram_data_width>, level_count - 1>>> delay_read_data;

    public:
        MemRead() {
            for (size_t i = 0; i < read_count; i++) {
                auto d_en = std::make_unique<Delay<bool, level_count>>(read_en[i], io.read_en_out[i]);
                delay_read_en.push_back(std::move(d_en));
                auto d_data = std::make_unique<Delay<UInt<sram_data_width>, level_count - 1>>(read_data[i], io.read_out[i]);
                delay_read_data.push_back(std::move(d_data));
            }
        }

        void reset() override {
            io.pipe.reset();
            io.key.reset();
            for (size_t i = 0; i < read_count; i++) {
                read_en[i] = false;
                read_data[i] = 0;
                io.read_en_out[i] = false;
                io.read_out[i] = 0;
                io.match_read[i].en = false;
                io.match_read[i].sram_id = 0;
                io.match_read[i].addr = 0;
            }
        }

        // 以组合逻辑生成读取的请求
        void update() override {
            if (io.pipe.in) {
                for (size_t i = 0; i < read_count; i++) {
                    read_en[i] = (MemReadConfig<mau_id>::mask(i) & io.gateway_in.get()) == MemReadConfig<mau_id>::mask(i);
                    io.match_read[i].en = read_en[i];
                    io.match_read[i].sram_id = io.sram_id_in[i];
                    io.match_read[i].addr = io.on_chip_addr_in[i];
                }
            }
            else {
                // 在没有数据包的情况下也不需要读取
                for (size_t i = 0; i < read_count; i++) {
                    io.match_read[i].en = false;
                    io.match_read[i].sram_id = 0;
                    io.match_read[i].addr = 0;
                }
            }
            // 无论有没有数据包都需要接收上一个周期读取的结果
            for (size_t i = 0; i < read_count; i++) {
                read_data[i] = io.match_read[i].data;
            }
        }

        void run() override {
            delay_phv.run();
            delay_key.run();
            for (size_t i = 0; i < read_count; i++) {
                delay_read_en[i]->run();
                delay_read_data[i]->run();
            }
        }

    };

}