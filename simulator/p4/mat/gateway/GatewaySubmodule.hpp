#pragma once

#include "../../../utils.hpp"
#include <map>
#include <functional>

namespace fpga::p4::mat::gateway {

    // 一个比较器，支持至多 32 位的比较
    struct GatewaySubmodule : public Module {

        struct IO {
            In<uint32_t> a, b;
            In<const char*> op;
            Out<bool> res;
        } io;

    private:

        struct cmp_str {
            bool operator()(const char* a, const char* b) const {
                return std::strcmp(a, b) < 0;
            }
        };

        // 操作符和比较函数的映射
        static const inline std::map < const char*, std::function<bool(int, int)>, cmp_str > gateway_ops = {
            { "eq" , [](int a, int b) { return a == b; } },
            { "neq", [](int a, int b) { return a != b; } },
            { "lt" , [](int a, int b) { return a < b; } },
            { "le" , [](int a, int b) { return a <= b; } },
            { "gt" , [](int a, int b) { return a > b; } },
            { "ge" , [](int a, int b) { return a >= b; } },
            { "ult", [](int a, int b) { return (unsigned int)a < (unsigned int)b; } },
            { "ule", [](int a, int b) { return (unsigned int)a <= (unsigned int)b; } },
            { "ugt", [](int a, int b) { return (unsigned int)a > (unsigned int)b; } },
            { "uge", [](int a, int b) { return (unsigned int)a >= (unsigned int)b; } }
        };

    public:

        void update() override {
            io.res = gateway_ops.at(io.op)((int)io.a, (int)io.b);
        }

    };

}