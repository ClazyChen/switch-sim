#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::getaddress {

    template <size_t mau_id>
    struct GetAddressChipSelect : public Module {

    private:
        static constexpr size_t group = fpga::Config::mat::getaddress::chip_select::group;
        static constexpr size_t vaddr_width = fpga::Config::mat::getaddress::chip_select::addr_width;
        static constexpr size_t paddr_width = fpga::Config::mat::getaddress::chip_select::data_width;

    public:

        struct IO {
            std::array<In<UInt<vaddr_width>>, group> vaddr_in;
            std::array<Out<UInt<paddr_width>>, group> paddr_out;
        } io;

        size_t unit_id = 0;

        GetAddressChipSelect() = default;
        GetAddressChipSelect(size_t unit_id) : unit_id(unit_id) {}

        void reset() override {
            for (size_t i = 0; i < group; i++) {
                io.paddr_out[i] = 0;
            }
        }

        void update() override {
            for (size_t i = 0; i < group; i++) {
                io.paddr_out[i] = Program<mau_id>::mat::getaddress::chip_select[
                    io.vaddr_in[i].get().value() + (unit_id << vaddr_width)
                ];
            }
        }

    };

}