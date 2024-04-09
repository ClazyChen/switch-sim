#pragma once

#include "TransformConfig.hpp"
#include "../../Pipe.hpp"

namespace fpga::p4::mat::transform {

    // 选出在 Action 阶段将要执行的 Value
    template <size_t mau_id>
    struct Transform : public Module {

    private:
        static constexpr size_t kv_pair_count = fpga::Config::mat::compare::kv_pair_count;
        static constexpr size_t value_count = fpga::Config::mat::transform::value_count;
        static constexpr auto& value_parallel = fpga::Config::mat::transform::value_parrallel;
        static constexpr size_t value_width = fpga::Config::mat::transform::value_width;

    public:
        struct IO {
            Pipe pipe;
            std::array<In<bool>, kv_pair_count> read_en_in;
            std::array<In<bool>, kv_pair_count> hit_in;
            std::array<In<UInt<value_width>>, kv_pair_count> value_in;
            std::array<Out<UInt<value_width>>, value_count> value_out;
        } io;

        void reset() override {
            io.pipe.reset();
            for (size_t i = 0; i < value_count; i++) {
                io.value_out[i] = 0;
            }
        }

        void run() override {
            if (io.pipe.in) {
                for (size_t i = 0, lo = 0; i < value_count; i++) {
                    size_t hi = lo + value_parallel[i];
                    // KV 对的来源在第 (hi, lo] 之间
                    bool valid = false;
                    bool hit = false;
                    for (size_t j = lo; j < hi; j++) {
                        if (io.read_en_in[j]) {
                            // 说明这个表确实有被读取
                            valid = true;
                        }
                        if (io.hit_in[j]) {
                            // 说明这个表被命中
                            hit = true;
                            io.value_out[i] = io.value_in[j];
                        }
                    }
                    // 如果这个表没有被读取，那么就不会有输出
                    if (!valid) {
                        io.value_out[i] = 0;
                    }
                    else if (!hit) {
                        // 如果这个表被读取了，但是没有被命中，则输出默认值
                        io.value_out[i] = TransformConfig<mau_id>::default_value(i);
                    }
                    lo = hi;
                }
            } else {
                for (size_t i = 0; i < value_count; i++) {
                    io.value_out[i] = 0;
                }
            }
        }

    };

}