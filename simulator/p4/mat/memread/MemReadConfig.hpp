#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::memread {

    template <size_t mau_id>
    struct MemReadConfig : public Module {

        static constexpr size_t gateway_width = fpga::Config::mat::gateway::width;

        static constexpr auto mask(size_t index) {
            return UInt<gateway_width>(Program<mau_id>::mat::memread::mask[index]);
        }

    };

}