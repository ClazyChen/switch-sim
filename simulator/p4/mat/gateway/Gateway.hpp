#pragma once

#include "GatewayCompare.hpp"
#include "GatewayTranslate.hpp"

namespace fpga::p4::mat::gateway {

    template <size_t mau_id>
    struct Gateway : public Module {

    private:
        static constexpr size_t gateway_out_width = fpga::Config::mat::gateway::translate::data_width;

    public:
        struct IO {
            Pipe pipe;
            In<LookupKey> lookup_key_in;
            In<GatewayKey> gateway_key_in;
            Out<LookupKey> key_out;
            Out<UInt<gateway_out_width>> gateway_out;
        } io;

        GatewayCompare<mau_id> cmp;
        GatewayTranslate<mau_id> trans;

        void reset() override {
            io.pipe.reset();
            io.key_out = nullptr;
            io.gateway_out = 0;
            cmp.reset();
            trans.reset();
        }

        void update() override {
            // 串联流水线接口
            io.pipe >> cmp.io.pipe;
            cmp.io.pipe > trans.io.pipe;

            // 其他连线
            cmp.io.lookup_key_in = io.lookup_key_in;
            cmp.io.gateway_key_in = io.gateway_key_in;
            trans.io.key_in = cmp.io.lookup_key_out;
            trans.io.gateway_in = cmp.io.gateway_out;

            // 执行内部更新
            cmp.update();
            trans.update();
        }

        void run() override {
            // 执行内部运行
            cmp.run();
            trans.run();

            // 输出到外部
            io.gateway_out = trans.io.gateway_out;
            io.key_out = trans.io.key_out;
            io.pipe << trans.io.pipe;
        }

    };

}