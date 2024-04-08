#pragma once

#include "GetAddressBitPool.hpp"
#include "GetAddressChipSelect.hpp"
#include "../../Pipe.hpp"
#include "../hash/HashValue.hpp"

namespace fpga::p4::mat::getaddress {

    template <size_t mau_id>
    struct GetAddress : public Module {

    private:
        static constexpr size_t hash_count = fpga::Config::mat::hash::count;
        static constexpr size_t gateway_width = fpga::Config::mat::gateway::width;
        static constexpr size_t read_count = fpga::Config::mat::getaddress::read_count;
        static constexpr size_t sram_id_width = fpga::Config::mat::getaddress::sram_id_width;
        static constexpr size_t on_chip_addr_width = fpga::Config::mat::getaddress::on_chip_addr_width;
        static constexpr size_t chip_select_count = fpga::Config::mat::getaddress::chip_select::count;
        static constexpr size_t chip_select_group = fpga::Config::mat::getaddress::chip_select::group;
        static constexpr size_t kv_pair_count = fpga::Config::mat::hash::value::kv_pair_count;
        static constexpr auto& write_to_phv = fpga::Config::mat::getaddress::write_to_phv;
        static constexpr size_t write_to_phv_width = fpga::Config::mat::getaddress::write_to_phv_width;

    public:

        struct IO {
            Pipe pipe;
            Piped<LookupKey> key;
            Piped<UInt<gateway_width>> gateway;
            std::array<In<fpga::p4::mat::hash::HashValue>, hash_count> hash_in;
            std::array<Out<UInt<sram_id_width>>, read_count> sram_id_out;
            std::array<Out<UInt<on_chip_addr_width>>, read_count> on_chip_addr_out;
        } io;

        std::array<Wire<UInt<sram_id_width>>, read_count> paddr;
        std::array<Wire<UInt<on_chip_addr_width>>, read_count> on_chip_addr;

        GetAddressBitPool<mau_id> bit_pool;
        std::array<GetAddressChipSelect<mau_id>, chip_select_count> chip_select;

    public:

        GetAddress() {
            for (size_t i = 0; i < chip_select_count; i++) {
                chip_select[i] = GetAddressChipSelect<mau_id>(i);
            }
        }

        void reset() override {
            io.pipe.reset();
            io.key.reset();
            io.gateway.reset();
            bit_pool.reset();
            for (size_t i = 0; i < chip_select_count; i++) {
                chip_select[i].reset();
            }
            for (size_t i = 0; i < read_count; i++) {
                io.sram_id_out[i] = 0;
                io.on_chip_addr_out[i] = 0;
                paddr[i] = 0;
                on_chip_addr[i] = 0;
            }
        }

        void update() override {
            if (io.pipe.in) {
                cal_on_chip_addr<0>();
                cal_vaddr();
                cal_paddr();
            }
        }

        void run() override {
            io.pipe.pass();
            io.key.pass();
            io.gateway.pass();
            if (io.pipe.in) {
                for (size_t i = 0; i < read_count; i++) {
                    io.sram_id_out[i] = paddr[i];
                    io.on_chip_addr_out[i] = on_chip_addr[i];
                }
                write_back<0>();
            }
        }

    private:

        // 计算第 I 个 KV 对
        template <size_t I> requires (I < read_count / 2)
            void cal_on_chip_addr() {
            on_chip_addr[I * 2] = io.hash_in[I / kv_pair_count].get().key<I % kv_pair_count>();
            on_chip_addr[I * 2 + 1] = io.hash_in[I / kv_pair_count].get().value<I % kv_pair_count>();
            if constexpr (I + 1 < read_count / 2) {
                cal_on_chip_addr<I + 1>();
            }
        }

        
        // 借助比特池计算虚拟地址
        void cal_vaddr() {
            for (size_t i = 0; i < hash_count; i++) {
                bit_pool.io.hash_in[i] = io.hash_in[i].get().chip_select();
            }
            bit_pool.update();
        }

        // 借助 CS 模块计算物理地址
        void cal_paddr() {
            for (size_t i = 0; i < chip_select_count; i++) {
                for (size_t j = 0; j < chip_select_group; j++) {
                    chip_select[i].io.vaddr_in[j] = bit_pool.io.vaddr_out[i * chip_select_group + j];
                }
                chip_select[i].update();
                for (size_t j = 0; j < chip_select_group; j++) {
                    paddr[i * chip_select_group + j] = chip_select[i].io.paddr_out[j];
                }
            }
        }

        // 将哈希结果写回到 PHV 的特定位置
        template <size_t I> requires (I < hash_count)
        void write_back() {
            if constexpr (write_to_phv[I]) {
                io.pipe.out.get().set_field<write_to_phv_width>(
                    Program<mau_id>::mat::getaddress::write_to_phv[I],
                    io.hash_in[I].get().asUInt()
                );
            }
            if constexpr (I + 1 < hash_count) {
                write_back<I + 1>();
            }
        }
    };

}