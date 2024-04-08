cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (simulator LANGUAGES CXX)

if (MSVC)
  add_compile_options(/utf-8)
  add_compile_options(/EHsc)
endif()

add_executable (simulator
    "simulator.cpp"
    "Config.hpp"
    "utils.hpp"
    "cases/basic/BasicAnalyzer.hpp"
    "cases/basic/BasicGenerator.hpp"
    "cases/basic/BasicSystem.hpp"
    "p4/FieldBundleProfileOf.hpp"
    "p4/Mau.hpp"
    "p4/Phv.hpp"
    "p4/Pipe.hpp"
    "p4/Pipeline.hpp"
    "p4/mat/Match.hpp"
    "p4/mat/gateway/Gateway.hpp"
    "p4/mat/gateway/GatewayCompare.hpp"
    "p4/mat/gateway/GatewayCompareConfig.hpp"
    "p4/mat/gateway/GatewaySubmodule.hpp"
    "p4/mat/gateway/GatewayTranslate.hpp"
    "p4/mat/gateway/GatewayTranslateConfig.hpp"
    "p4/mat/getaddress/GetAddress.hpp"
    "p4/mat/getaddress/GetAddressBitPool.hpp"
    "p4/mat/getaddress/GetAddressChipSelect.hpp"
    "p4/mat/getkey/GetKey.hpp"
    "p4/mat/getkey/GetKeyConfig.hpp"
    "p4/mat/hash/Hash.hpp"
    "p4/mat/hash/HashUnit.hpp"
    "p4/mat/hash/HashUnitConfig.hpp"
    "p4/mat/hash/HashValue.hpp"
    "program/Program.hpp"
    "test/Console.hpp"
    "test/TestApplication.hpp"
    "test/TestInstance.hpp"
    "utils/Delay.hpp"
    "utils/FieldBundle.hpp"
    "utils/FieldBundleProfile.hpp"
    "utils/Hex.hpp"
    "utils/Input.hpp"
    "utils/InputAnalyzer.hpp"
    "utils/Module.hpp"
    "utils/Output.hpp"
    "utils/Packet.hpp"
    "utils/PacketGenerator.hpp"
    "utils/Piped.hpp"
    "utils/Random.hpp"
    "utils/UInt.hpp"
    "utils/Wire.hpp"
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET simulator PROPERTY CXX_STANDARD 20)
endif()
