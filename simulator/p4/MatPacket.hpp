#pragma once

#include "../utils/Packet.hpp"
#include "../Config.hpp"
#include "../utils/FieldBundle.hpp"
#include "../utils/FieldBundleProfile.hpp"
#include "FieldBundleProfileOf.hpp"
#include <array>

namespace fpga::p4 {
    struct MatPacket : public fpga::utils::Packet, public fpga::utils::FieldBundle<fpga::p4::FieldBundleProfileOf::phv> {

        MatPacket() : Packet() {}

        MatPacket(
            const std::array<int, count8> field8_,
            const std::array<int, count16> field16_,
            const std::array<int, count32> field32_,
            int payload_cycles,
            long long arrive_time

        ) : Packet(arrive_time, payload_cycles), FieldBundle(field8_, field16_, field32_) {

        }
    };


    //struct Phv : public FieldBundle<FieldBundleProfileOf::phv> {

    //    // 返回一个全 0 的phv，用于作为无效输入的默认值
    //    static std::shared_ptr<Phv> zero() {
    //        return std::make_shared<Phv>();
    //    }
    //};

}