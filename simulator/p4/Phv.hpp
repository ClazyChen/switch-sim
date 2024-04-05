#pragma once

#include "../utils.hpp"
#include "FieldBundleProfileOf.hpp"

namespace fpga::p4 {

    struct Phv : public FieldBundle<FieldBundleProfileOf::phv> {

        // 数据包 ID 等元数据
        Packet meta;

        // 未来在此处添加更多的字段，如 TO_CPU 等

    };

}