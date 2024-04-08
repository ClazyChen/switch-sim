#pragma once

#include "getkey/GetKey.hpp"
#include "gateway/Gateway.hpp"
#include "hash/Hash.hpp"

namespace fpga::p4::mat {

    template <size_t mau_id>
    struct Match : public Module {
        struct IO {
            Pipe pipe;
        } io;

        // 每一级流水线
        getkey::Getkey<mau_id> getkey;
        gateway::Gateway<mau_id> gateway;
        hash::Hash<mau_id> hash;

        void reset() override {
            io.pipe.reset();
            getkey.reset();
            gateway.reset();
            hash.reset();
        }

        void update() override {
            // 传入每一级流水线的 input
            io.pipe >> getkey.io.pipe;
            getkey.io.pipe > gateway.io.pipe;
            gateway.io.pipe > hash.io.pipe;

            gateway.io.lookup_key_in = getkey.io.lookup_key_out;
            gateway.io.gateway_key_in = getkey.io.gateway_key_out;

            // 运行每一级流水线的 update
            getkey.update();
            gateway.update();
            hash.update();
        }

        void run() override {
            // 运行每一级流水线的 run
            getkey.run();
            gateway.run();
            hash.run();

            // 将最后一级流水线的输出传递给出去
            io.pipe << hash.io.pipe;

        }

    };
}