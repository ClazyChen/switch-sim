#pragma once

#include <array>

namespace fpga {
    struct Config {
        static constexpr int axis_width = 512;
        static constexpr int macpu_count = 4;
        static constexpr int field_type_count = 3;
        struct phv {
            static constexpr int field8_count = 64;
            static constexpr int field16_count = 96;
            static constexpr int field32_count = 64;
        };
        struct ring {
            static constexpr int delay = 2;
        };
        struct memory {
            struct sram {
                static constexpr int count = 80;
                static constexpr int addr_width = 10;
                static constexpr int data_width = 128;
                static constexpr int ctrl_write_count = 1;
            };
        };
        struct parser {
            static constexpr int header_width = 4096;
            static constexpr int state_id_width = 8;
            static constexpr int concat_id_width = 6;
            static constexpr int packet_id_width = 16;
            static constexpr int packet_id_container = 0;
            static constexpr int sram_addr_width = 8;
        };
        struct payload {
            static constexpr int sram_addr_width = 12;
            static constexpr int payload_fifo_depth = 64;
        };
        struct macpu {
            static constexpr bool enable = true;
            static constexpr int width = 32;
            static constexpr int count = 8;
            static constexpr int core_id_width = 8;
        };
        struct mat {
            struct getkey {
                static constexpr std::array key_names = { "lut", "gtw" };
                struct key_lut {
                    static constexpr int field8_count = 32;
                    static constexpr int field16_count = 16;
                    static constexpr int field32_count = 16;
                };
                struct key_gtw {
                    static constexpr int field8_count = 16;
                    static constexpr int field16_count = 8;
                    static constexpr int field32_count = 8;
                };
                static constexpr bool any_connect = false;
            };
            struct gateway {
                static constexpr int gateway8_count = 8;
                static constexpr int gateway16_count = 4;
                static constexpr int gateway32_count = 4;
                static constexpr auto all_count = gateway8_count + gateway16_count + gateway32_count;
                static constexpr std::array ops = { "eq", "ne", "lt", "gt", "le", "ge" };
                static constexpr bool any_connect = false;
                struct translate {
                    static constexpr int addr_width = 8;
                    static constexpr int data_width = 16;
                };
                static constexpr auto width = translate::data_width;
            };
            struct hash {
                static constexpr int count = 4;
                static constexpr std::array fixed = { false, false, false, false };
                static constexpr int unit_size = 128;
                static constexpr std::array merge_levels = { 128, 8 };
                struct value {
                    static constexpr int chip_select_width = 12;
                    static constexpr bool kv_shared = true;
                    static constexpr int kv_pair_count = 2;
                    static constexpr auto kv_addr_width = Config::memory::sram::addr_width;
                    static constexpr auto width = chip_select_width + kv_pair_count * kv_addr_width * (kv_shared ? 1 : 2);
                };
            };
            struct getaddress {
                static constexpr auto read_count = Config::mat::hash::count * Config::mat::hash::value::kv_pair_count * (Config::mat::hash::value::kv_shared ? 2 : 1);
                struct chip_select {
                    static constexpr int count = 4;
                    static constexpr auto group = read_count / count;
                    static constexpr int addr_width = 5;
                    static constexpr int data_width = 7;
                    static constexpr auto total_width = (1 << addr_width) * data_width;
                };
                struct bit_pool {
                    static constexpr bool explicit_zero = true;
                    static constexpr bool explicit_one = true;
                    static constexpr auto size = Config::mat::hash::count * Config::mat::hash::value::chip_select_width + (explicit_zero ? 1 : 0) + (explicit_one ? 1 : 0);
                    static constexpr auto output_width = read_count * chip_select::addr_width;
                };
                static constexpr auto sram_id_width = chip_select::data_width;
                static constexpr auto on_chip_addr_width = Config::memory::sram::addr_width;
                static constexpr std::array write_to_phv = { true, true, true, true };
                static constexpr int write_to_phv_width = 32;
            };
            struct memread {
                static constexpr int level_count = 2;
                static constexpr auto table_count = Config::mat::getaddress::chip_select::count;
                static constexpr auto sram_data_width = Config::memory::sram::data_width;
            };
            struct compare {
                static constexpr auto kv_pair_count = Config::mat::hash::value::kv_pair_count * Config::mat::hash::count;
                static constexpr std::array key_part_start = { 0, 128, 256, 384, 512, 640, 768, 896 };
                static constexpr std::array key_id_in_reads = { 0, 2, 4, 6, 8, 10, 12, 14 };
                static constexpr int mask_unit = 8;
                static constexpr auto mask_width = Config::memory::sram::data_width / mask_unit;
            };
            struct transform {
                static constexpr int value_count = 4;
                static constexpr auto value_width = Config::memory::sram::data_width;
                static constexpr std::array value_parrallel = { 2, 2, 2, 2 };
            };
        };
        struct act {
            struct insfetch {
                struct value {
                    static constexpr int action_id_width = 32;
                    static constexpr int argument_width = 32;
                    static constexpr auto argument_count = (Config::mat::transform::value_width - action_id_width) / argument_width;
                };
                static constexpr auto vliw_count = Config::mat::transform::value_count;
                static constexpr int vliw_width = 512;
                struct sram {
                    static constexpr int addr_width = 10;
                    static constexpr auto data_width = (Config::macpu::enable ? Config::macpu::width : Config::act::insfetch::vliw_width);
                    static constexpr auto count = Config::act::insfetch::vliw_width / data_width;
                };
                static constexpr auto argument_count = Config::act::insfetch::value::argument_count * vliw_count;
                static constexpr int primitive_width = 64;
                static constexpr auto primitive_count_per_vliw = vliw_width / primitive_width;
                static constexpr auto primitive_count = vliw_count * primitive_count_per_vliw;
                static constexpr auto cpu_count_per_vliw = Config::macpu::count / Config::mat::transform::value_count;
            };
            struct distribute {
                static constexpr bool direct_connect = false;
                static constexpr int location_width = 6;
                static constexpr auto distributed_width = Config::act::insfetch::primitive_width - location_width;
                static constexpr bool independent_stage = true;
            };
            struct executer {
                static constexpr int pipeline_level = 3;
                static constexpr int rd_width = 8;
                struct alu {
                    static constexpr int alu8_count = 8;
                    static constexpr int alu16_count = 8;
                    static constexpr int alu32_count = 8;
                    static constexpr auto all_count = alu8_count + alu16_count + alu32_count;
                };
                struct salu {
                    static constexpr int alu8s_count = 0;
                    static constexpr int alu16s_count = 0;
                    static constexpr int alu32s_count = 8;
                    static constexpr auto all_count = alu8s_count + alu16s_count + alu32s_count;
                };
                static constexpr auto parallel_size = alu::all_count + salu::all_count;
            };
        };
        struct programmed {
            static constexpr bool enable = false;
        };
        struct debug {
            static constexpr bool enable = true;
            struct phv_test {
                static constexpr int line_limit = 1024;
            };
        };
    };
}
