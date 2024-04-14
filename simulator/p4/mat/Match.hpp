#pragma once

#include "getkey/GetKey.hpp"
#include "gateway/Gateway.hpp"
#include "hash/Hash.hpp"
#include "getaddress/GetAddress.hpp"
#include "memread/MemRead.hpp"
#include "compare/Compare.hpp"
#include "transform/Transform.hpp"

namespace fpga::p4::mat {

    template <size_t mau_id>
    struct Match : public Module {
        
    private:
        static constexpr size_t read_count = fpga::Config::mat::getaddress::read_count;
        static constexpr size_t value_count = fpga::Config::mat::transform::value_count;
        static constexpr size_t value_width = fpga::Config::mat::transform::value_width;

    public:
        struct IO {
            Pipe pipe;
            std::array<Out<UInt<value_width>>, value_count> value_out;
            std::array<fpga::p4::mem::SramClusterRead, read_count> match_read;
        } io;

        // 每一级流水线
        getkey::Getkey<mau_id> getkey;
        gateway::Gateway<mau_id> gateway;
        hash::Hash<mau_id> hash;
        getaddress::GetAddress<mau_id> getaddress;
        memread::MemRead<mau_id> memread;
        compare::Compare<mau_id> compare;
        transform::Transform<mau_id> transform;

        void reset() override {
            io.pipe.reset();
            getkey.reset();
            gateway.reset();
            hash.reset();
            getaddress.reset();
            memread.reset();
            compare.reset();
            transform.reset();
        }

        void update() override {
            // 传入每一级流水线的 input
            io.pipe >> getkey.io.pipe;
            getkey.io.pipe > gateway.io.pipe;
            gateway.io.pipe > hash.io.pipe;
            hash.io.pipe > getaddress.io.pipe;
            getaddress.io.pipe > memread.io.pipe;
            memread.io.pipe > compare.io.pipe;
            compare.io.pipe > transform.io.pipe;

            // 其他连线
            gateway.io.lookup_key_in = getkey.io.lookup_key_out;
            gateway.io.gateway_key_in = getkey.io.gateway_key_out;

            hash.io.key.in = gateway.io.key_out;
            hash.io.gateway.in = gateway.io.gateway_out;

            getaddress.io.key.in = hash.io.key.out;
            getaddress.io.gateway.in = hash.io.gateway.out;
            std::copy(hash.io.hash_out.begin(), hash.io.hash_out.end(), getaddress.io.hash_in.begin());

            memread.io.key.in = getaddress.io.key.out;
            memread.io.gateway_in = getaddress.io.gateway.out;
            std::copy(getaddress.io.sram_id_out.begin(), getaddress.io.sram_id_out.end(), memread.io.sram_id_in.begin());
            std::copy(getaddress.io.on_chip_addr_out.begin(), getaddress.io.on_chip_addr_out.end(), memread.io.on_chip_addr_in.begin());
            for (int i = 0; i < read_count; i++) {
                io.match_read[i].en.get() = memread.io.match_read[i].en.get();
                io.match_read[i].sram_id.get() = memread.io.match_read[i].sram_id.get();
                io.match_read[i].addr.get() = memread.io.match_read[i].addr.get();
                memread.io.match_read[i].data.get() = io.match_read[i].data.get();
            }

            compare.io.key_in = memread.io.key.out;
            std::copy(memread.io.read_en_out.begin(), memread.io.read_en_out.end(), compare.io.read_en_in.begin());
            std::copy(memread.io.read_out.begin(), memread.io.read_out.end(), compare.io.read_in.begin());

            std::copy(compare.io.read_en_out.begin(), compare.io.read_en_out.end(), transform.io.read_en_in.begin());
            std::copy(compare.io.hit_out.begin(), compare.io.hit_out.end(), transform.io.hit_in.begin());
            std::copy(compare.io.value_out.begin(), compare.io.value_out.end(), transform.io.value_in.begin());

            // 运行每一级流水线的 update
            getkey.update();
            gateway.update();
            hash.update();
            getaddress.update();
            memread.update();
            compare.update();
            transform.update();

            // 传递组合逻辑的输出
            std::copy(memread.io.match_read.begin(), memread.io.match_read.end(), io.match_read.begin());
        }

        void run() override {
            // 运行每一级流水线的 run
            getkey.run();
            gateway.run();
            hash.run();
            getaddress.run();
            memread.run();
            compare.run();
            transform.run();

            // 将最后一级流水线的输出传递给出去
            io.pipe << transform.io.pipe;

            // 传递查到的值，用于后续的处理
            std::copy(transform.io.value_out.begin(), transform.io.value_out.end(), io.value_out.begin());
        }

    };
}