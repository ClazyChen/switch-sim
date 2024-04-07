#pragma once
#include "MatPacket.hpp"
#include "../test/TestInstance.hpp"
#include "MatInput.hpp"
#include "MatOutput.hpp"
#include "../utils/FieldBundle.hpp"
#include "../utils/FieldBundleProfile.hpp"
#include "mat/getkey/GetKey.hpp"
#include <array>
#include "../Config.hpp"

namespace fpga::p4 {
    template<
        size_t FIELD8_NUM,
        size_t FIELD16_NUM,
        size_t FIELD32_NUM
    >
    struct MatSystem : public fpga::test::TestInstance {
        //fpga::mat::MatInput<FIELD8_NUM, FIELD16_NUM, FIELD32_NUM> input;
        fpga::p4::MatInput<FIELD8_NUM, FIELD16_NUM, FIELD32_NUM, fpga::Config> input;
        fpga::p4::MatOutput<FIELD8_NUM, FIELD16_NUM, FIELD32_NUM> output;
        fpga::p4::mat::getkey::Getkey<0> getkey;

        void initialize(const char* input_file, const char* output_file) override {
            input.initialize(input_file);
            output.initialize(output_file);
        }

        void report() override {

        }

        void reset() override {

        }

        void update() override {
            getkey.io.packet_in = input.io.packet_out;
            getkey.update();

        }

        void run() override {
            input.run();
            output.run();
            getkey.run();
        }
    };

}