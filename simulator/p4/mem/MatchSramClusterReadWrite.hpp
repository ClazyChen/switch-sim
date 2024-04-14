#pragma once

#include "../../utils.hpp"

namespace fpga::p4::mem {

    struct MatchSramClusterRead {

        static constexpr size_t sram_id_width = fpga::Config::mat::getaddress::sram_id_width;
        static constexpr size_t addr_width = fpga::Config::memory::sram::addr_width;
        static constexpr size_t data_width = fpga::Config::memory::sram::data_width;

        Piped<bool> en;
        Piped<UInt<sram_id_width>> sram_id;
        Piped<UInt<addr_width>> addr;
        Piped<UInt<data_width>> data;

        /*Out<bool> en_out;
        Out<UInt<sram_id_width>> sram_id_out;
        Out<UInt<addr_width>> addr_out;
        In<UInt<data_width>> data_in;*/

    };

    struct MatchSramClusterWrite {

        static constexpr size_t sram_id_width = fpga::Config::mat::getaddress::sram_id_width;
        static constexpr size_t addr_width = fpga::Config::memory::sram::addr_width;
        static constexpr size_t data_width = fpga::Config::memory::sram::data_width;

        Out<bool> en;
        Out<UInt<sram_id_width>> sram_id;
        Out<UInt<addr_width>> addr;
        Out<UInt<data_width>> data;

    };
}