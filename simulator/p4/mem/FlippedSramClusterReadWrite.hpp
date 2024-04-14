#pragma once

#include "../../utils.hpp"

namespace fpga::p4::mem {

    struct FlippedSramClusterRead {

        static constexpr size_t sram_id_width = fpga::Config::mat::getaddress::sram_id_width;
        static constexpr size_t addr_width = fpga::Config::memory::sram::addr_width;
        static constexpr size_t data_width = fpga::Config::memory::sram::data_width;

        Out<bool> en;
        Out<UInt<sram_id_width>> sram_id;
        Out<UInt<addr_width>> addr;
        In<UInt<data_width>> data;

    };

    struct FlippedSramClusterWrite {

        static constexpr size_t sram_id_width = fpga::Config::mat::getaddress::sram_id_width;
        static constexpr size_t addr_width = fpga::Config::memory::sram::addr_width;
        static constexpr size_t data_width = fpga::Config::memory::sram::data_width;

        Out<bool> en;
        Out<UInt<sram_id_width>> sram_id;
        Out<UInt<addr_width>> addr;
        Out<UInt<data_width>> data;

    };
}