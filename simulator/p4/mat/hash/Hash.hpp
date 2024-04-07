#pragma once

#include "HashUnit.hpp"
#include "HashValue.hpp"
#include "../../Pipe.hpp"

namespace fpga::p4::mat::hash {

    template <size_t mau_id>
    struct Hash : public Module {
    private:
        static constexpr size_t count = fpga::Config::mat::hash::count;
        static constexpr size_t gateway_width = fpga::Config::mat::gateway::width;
        static constexpr size_t key_width = LookupKey::width;
        static constexpr size_t value_width = fpga::Config::mat::hash::value::width;
        static constexpr size_t pipeline_level_count = fpga::Config::mat::hash::merge_levels.size();

    public:
        struct IO {
            Pipe pipe;
            Piped<LookupKey> key;
            Piped<UInt<gateway_width>> gateway;
            std::array<Out<HashValue>, count> hash_out;
        } io;

        std::array<std::unique_ptr<Module>, count> units;

    private:
        std::array<In<UInt<key_width>>*, count> units_in;
        std::array<Out<UInt<value_width>>*, count> units_out;

        template <size_t I> requires (I >= 0 && I < count)
        void init() {
            auto unit = std::make_unique<HashUnit<mau_id, I>>();
            units_in[I] = &unit->io.in;
            units_out[I] = &unit->io.out;
            units[I] = std::move(unit);
            if constexpr (I < count - 1) {
                init<I + 1>();
            }
        }

        std::array<Wire< HashValue>, count> temp_hash;

        Delay<Phv, pipeline_level_count, In> delay_phv{ io.pipe };
        Delay<LookupKey, pipeline_level_count, In> delay_key{ io.key };
        Delay<UInt<gateway_width>, pipeline_level_count, In> delay_gateway{ io.gateway };
        // std::vector<std::unique_ptr<Delay<HashValue, pipeline_level_count>>> delay_hash;

    public:
        Hash() {
            init<0>();
            /*for (size_t i = 0; i < count; i++) {
                delay_hash.push_back(
                    std::make_unique<Delay<HashValue, pipeline_level_count>>(temp_hash[i], io.hash_out[i]);
                );
            }*/
        }

        void reset() override {
            io.pipe.reset();
            io.key.reset();
            io.gateway.reset();
            for (size_t i = 0; i < count; i++) {
                temp_hash[i] = 0;
                io.hash_out[i] = 0;
                units[i]->reset();
            }
        }

        void update() override {
            if (io.pipe.in) {
                for (size_t i = 0; i < count; i++) {
                    *units_in[i] = io.key.in.get().asUInt();
                    units[i]->update();
                    temp_hash[i].get() = *units_out[i];
                }
            }
        }

        void run() override {
            delay_phv.run();
            delay_gateway.run();
            delay_key.run();
            /*for (auto& delay : delay_hash) {
                delay->run();
            }*/
        }
    };

}