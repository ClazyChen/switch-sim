#pragma once

#include "utils/Wire.hpp"
#include "utils/UInt.hpp"
#include "utils/Delay.hpp"
#include "utils/FieldBundle.hpp"
#include "utils/Input.hpp"
#include "utils/Output.hpp"
#include "utils/Random.hpp"
#include "utils/Packet.hpp"
#include "utils/Module.hpp"
#include "utils/PacketGenerator.hpp"
#include "utils/InputAnalyzer.hpp"
#include "utils/Hex.hpp"
#include "utils/Piped.hpp"
#include "utils/Sram.hpp"
#include "utils/Log2Ceil.hpp"

#include "Config.hpp"
#include "program/Program.hpp"

namespace fpga {
    using namespace utils;

    template <size_t mau_id>
    using Program = fpga::program::Program<mau_id>;
}