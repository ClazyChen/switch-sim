#pragma once

#include "../utils.hpp"
#include "FieldBundleProfileOf.hpp"

namespace fpga::p4 {

    struct Phv : public FieldBundle<FieldBundleProfileOf::phv>,
                 public Packet {

        // 未来在此处添加更多的字段，如 TO_CPU 等
            
        Phv() : FieldBundle<FieldBundleProfileOf::phv>(), Packet () {}

        Phv(size_t arrive_time, size_t payload_cycle) : FieldBundle<FieldBundleProfileOf::phv>(), Packet (arrive_time, payload_cycle) {}

    };

}