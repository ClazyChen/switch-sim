#pragma once

#include "GatewayTranslateConfig.hpp"
#include "../../Pipe.hpp"

namespace fpga::p4::mat::gateway {

    template <size_t mau_id>
    struct GatewayTranslate : public Module {

    private:
        static constexpr auto gateway_profile = FieldBundleProfileOf::gateway;
        static constexpr size_t count8 = gateway_profile.field8_count;
        static constexpr size_t count16 = gateway_profile.field16_count;
        static constexpr size_t count32 = gateway_profile.field32_count;
        static constexpr size_t gateway_count = gateway_profile.sum();
        static constexpr size_t gateway_out_width = fpga::Config::mat::gateway::translate::data_width;

    public:
        struct IO {
            Pipe pipe;
            In<LookupKey> key_in;
            Out<LookupKey> key_out;
            In<UInt<gateway_count>> gateway_in;
            Out<UInt<gateway_out_width>> gateway_out;
        } io;

        void reset() override {
            io.pipe.reset();
            io.key_out = nullptr;
            io.gateway_out = 0;
        }

        void run() override {
            if (io.pipe.in) {
                auto& value = io.gateway_in.get();
                io.gateway_out = 
                    GatewayTranslateConfig<mau_id>::translate(
                        value.slice<15, 8>() + 
                        value.slice<7, 0>()
                    );
            }
            else {
                io.gateway_out = 0;
            }
            io.pipe.pass();
            io.key_out = io.key_in;
        }

    };

}