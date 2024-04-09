#pragma once

#include "CompareConfig.hpp"
#include "../../Pipe.hpp"

namespace fpga::p4::mat::compare {

    // 将存储器中取出的数据和之前提取的 key 的对应字段比较
    // 以支持多路并行查找（布谷鸟哈希），避免因为哈希冲突导致的数据错误
    // 布谷鸟哈希由上层的控制平面进行维护，数据平面不需要关心
    template <size_t mau_id>
    struct Compare : public Module {

    private:
        static constexpr size_t read_count = fpga::Config::mat::getaddress::read_count;
        static constexpr size_t kv_pair_count = fpga::Config::mat::compare::kv_pair_count;
        static constexpr size_t sram_data_width = fpga::Config::mat::memread::sram_data_width;
        static constexpr auto& key_part_start = fpga::Config::mat::compare::key_part_start;
        static constexpr size_t key_part_count = key_part_start.size();
        static constexpr auto& key_id_in_reads = fpga::Config::mat::compare::key_id_in_reads;
        static constexpr size_t mask_unit = fpga::Config::mat::compare::mask_unit;

    public:
        struct IO {
            Pipe pipe;
            In<LookupKey> key_in;
            std::array<In<bool>, read_count> read_en_in;
            std::array<In<UInt<sram_data_width>>, read_count> read_in;
            std::array<Out<bool>, kv_pair_count> hit_out;
            std::array<Out<UInt<sram_data_width>>, kv_pair_count> value_out;
        } io;

        // 将 LookupKey 拆分成多个部分
        Wire<UInt<LookupKey::width>> key;
        std::array<Wire<UInt<sram_data_width>>, key_part_count> parts;

        void reset() override {
            io.pipe.reset();
            for (size_t i = 0; i < kv_pair_count; i++) {
                io.hit_out[i] = false;
                io.value_out[i] = 0;
            }
            key = 0;
            for (size_t i = 0; i < key_part_count; i++) {
                parts[i] = 0;
            }
        }

    private:
        
        // 将 key 转换为 Wire
        void key_to_wire() {
            key = io.key_in.get().asUInt();
        }

        // 将 LookupKey 拆分的结果放到 parts 中
        template <size_t I> requires (I < key_part_count)
            void split_key() {
            parts[I] = key.get().slice<key_part_start[I] + sram_data_width - 1, key_part_start[I]>();
            if constexpr (I + 1 < key_part_count) {
                split_key<I + 1>();
            }
        }

        // 判断哪些路是 key，哪些路是 value
        auto split_kv() {
            std::vector<size_t> read_key;
            std::vector<size_t> read_value;
            for (size_t i = 0; i < read_count; i++) {
                if (std::find(key_id_in_reads.begin(), key_id_in_reads.end(), i) != key_id_in_reads.end()) {
                    read_key.push_back(i);
                }
                else {
                    read_value.push_back(i);
                }
            }
            return std::make_pair(read_key, read_value);
        }

    public:
        void update() override {
            if (io.pipe.in) {
                key_to_wire();
                split_key<0>();
            }
        }

        void run() override {

            if (io.pipe.in) {
                auto [read_key, read_value] = split_kv();
                // 对 key 依次进行比较，value 保持不变
                for (size_t i = 0; i < kv_pair_count; i++) {
                    auto& part = parts[CompareConfig<mau_id>::key_part_index(i)].get();
                    auto mask = CompareConfig<mau_id>::mask(i);
                    io.hit_out[i] = io.read_en_in[i] && ((io.read_in[read_key[i]].get() & mask) == (part & mask));
                    io.value_out[i] = io.read_in[read_value[i]];
                }
            }
            else {
                for (size_t i = 0; i < kv_pair_count; i++) {
                    io.hit_out[i] = false;
                    io.value_out[i] = 0;
                }
            }
            io.pipe.pass();

        }

    };

}