#pragma once

#include "../utils/Output.hpp"
#include "MatPacket.hpp"
#include <vector>
#include <iostream>

namespace fpga::p4 {
    template<
        size_t FIELD8_NUM,
        size_t FIELD16_NUM,
        size_t FIELD32_NUM
    >
    struct MatOutput : public fpga::utils::Output<fpga::p4::MatPacket> {


        void reset() override {

        }

        void update() override {

        }

        void drop(const MatPacket& packet) {

        }

        void run() override {

        }

        // 报告结果
        void report() override {

        }

    };
}