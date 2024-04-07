#pragma once

#include "GatewayCompareConfig.hpp"
#include "GatewaySubmodule.hpp"
#include "../../Pipe.hpp"

namespace fpga::p4::mat::gateway {

    // 用于对 GatewayKey 进行比较的配置
    template <size_t mau_id>
    struct GatewayCompare : public Module {
        
    private:
        static constexpr auto gateway_profile = FieldBundleProfileOf::gateway;
        static constexpr size_t count8 = gateway_profile.field8_count;
        static constexpr size_t count16 = gateway_profile.field16_count;
        static constexpr size_t count32 = gateway_profile.field32_count;
        static constexpr size_t gateway_count = gateway_profile.sum();

    public:
        struct IO {
            Pipe pipe;
            In<LookupKey> lookup_key_in;
            In<GatewayKey> gateway_key_in;
            Out<LookupKey> lookup_key_out;
            Out<UInt<gateway_count>> gateway_out;
        } io;

        std::array<GatewaySubmodule, gateway_count> submodules;

        void reset() override {
            io.pipe.reset();
            io.lookup_key_out = nullptr;
            io.gateway_out = 0;
            for (auto &submodule : submodules) {
                submodule.reset();
            }
        }

        void update() override {
            if (io.pipe.in) {
                update_submodules();
            }
            // 如果没有输入，直接不启用 submodule
        }

        void run() override {
            if (io.pipe.in) {
                for (size_t i = 0; i < gateway_count; i++) {
                    io.gateway_out.get().set(i, submodules[i].io.res);
                }
            }
            else {
                io.gateway_out = 0;
            }
            io.pipe.pass();
            io.lookup_key_out = io.lookup_key_in;
        }

    private:

        // gateway_id 是同一宽度内的编号
        // submodule_id 是全局的编号
        template <size_t width>
        requires (width == 8 || width == 16 || width == 32)
        void update_gateway_module(size_t gateway_id, size_t submodule_id) {
            auto& submodule = submodules[submodule_id];
            auto [a, b, op] = GatewayCompareConfig<mau_id>::config<width>(gateway_id);
            submodule.io.a = io.gateway_key_in.get().field<width>(a);
            submodule.io.b = io.gateway_key_in.get().field<width>(b);
            submodule.io.op = op;
            submodule.update();
        }

        void update_submodules() {
            for (size_t i = 0; i < count8; i++) {
                update_gateway_module<8>(i, i);
            }
            for (size_t i = 0; i < count16; i++) {
                update_gateway_module<16>(i, i + count8);
            }
            for (size_t i = 0; i < count32; i++) {
                update_gateway_module<32>(i, i + count8 + count16);
            }
        }

    };

}