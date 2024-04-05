#pragma once

#include "../utils.hpp"

namespace fpga::p4 {

    // C++ 不支持反射
    // 因此如果要增加一个新的 FieldBundleProfile，需要在这里手动添加

    struct FieldBundleProfileOf {
        static constexpr FieldBundleProfile phv = {
            fpga::Config::phv::field8_count,
            fpga::Config::phv::field16_count,
            fpga::Config::phv::field32_count
        };
        static constexpr FieldBundleProfile key_lut = {
            fpga::Config::mat::getkey::key_lut::field8_count,
            fpga::Config::mat::getkey::key_lut::field16_count,
            fpga::Config::mat::getkey::key_lut::field32_count
        };
        static constexpr FieldBundleProfile key_gtw = {
            fpga::Config::mat::getkey::key_gtw::field8_count,
            fpga::Config::mat::getkey::key_gtw::field16_count,
            fpga::Config::mat::getkey::key_gtw::field32_count
        };
        static constexpr FieldBundleProfile gateway = {
            fpga::Config::mat::gateway::gateway8_count,
            fpga::Config::mat::gateway::gateway16_count,
            fpga::Config::mat::gateway::gateway32_count
        };
    };

    using LookupKey = FieldBundle<FieldBundleProfileOf::key_lut>;
    using GatewayKey = FieldBundle<FieldBundleProfileOf::key_gtw>;

}