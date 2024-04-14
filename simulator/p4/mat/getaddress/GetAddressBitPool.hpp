#pragma once

#include "../../../utils.hpp"

namespace fpga::p4::mat::getaddress {

    template <size_t mau_id>
    struct GetAddressBitPool : public Module {
    private:
        static constexpr bool explicit_zero = fpga::Config::mat::getaddress::bit_pool::explicit_zero;
        static constexpr bool explicit_one = fpga::Config::mat::getaddress::bit_pool::explicit_one;
        static constexpr size_t bit_pool_size = fpga::Config::mat::getaddress::bit_pool::size;
        static constexpr size_t output_width = fpga::Config::mat::getaddress::bit_pool::output_width;
        static constexpr size_t hash_count = fpga::Config::mat::hash::count;
        static constexpr size_t hash_cs_width = fpga::Config::mat::hash::value::chip_select_width;
        static constexpr size_t read_count = fpga::Config::mat::getaddress::read_count;
        static constexpr size_t vaddr_width = fpga::Config::mat::getaddress::chip_select::addr_width;

    public:

        struct IO {
            std::array<In<UInt<hash_cs_width>>, hash_count> hash_in;
            std::array<Out<UInt<vaddr_width>>, read_count> vaddr_out;
        } io;

        void reset() override {
            for (auto& vaddr : io.vaddr_out) {
                vaddr = 0;
            }
        }

        void update() override {
            for (size_t i = 0; i < read_count; i++) {
                for (size_t j = 0; j < vaddr_width; j++) {
                    io.vaddr_out[i].get().set(j, bit_pool(
                        Program<mau_id>::mat::getaddress::bit_pool[i * vaddr_width + j]
                    ));
                }
            }
        }

    private:

        // 返回比特池中的某一位
        constexpr uint64_t bit_pool(size_t index) {
            if (index < hash_cs_width * hash_count) {
                return io.hash_in[index / hash_cs_width].get()(index % hash_cs_width);
            }
            else {
                index -= hash_cs_width * hash_count;
                if constexpr (explicit_zero) {
                    if (index == 0) {
                        return 0;
                    }
                }
            }
        }
    };

}