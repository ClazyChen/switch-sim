#pragma once
#include <iostream>
#include <bit>

#include "HashUnitConfig.hpp"

namespace fpga::p4::mat::hash {

    template <size_t mau_id, size_t unit_id>
    struct HashUnit : public Module {
        static constexpr size_t key_width = LookupKey::width;
        static constexpr size_t value_width = fpga::Config::mat::hash::value::width;
        
        struct IO {
            In<UInt<key_width>> in;
            Out<UInt<value_width>> out;
        } io;

        void reset() override {
            io.out = 0;
        }

        void update() override {
            auto key = io.in.get() & HashUnitConfig<mau_id>::get().masks[unit_id];
            for (size_t i = 0; i < value_width; i++) {
                int popcount = ((key ^ HashUnitConfig<mau_id>::get().matrices[unit_id][i])).xorR() % 2;
                io.out.get().set(i, popcount);
            }
            io.out.get() ^= HashUnitConfig<mau_id>::get().salts[unit_id];
        }

    };

}