#pragma once

#include "TestInstance.hpp"
#include "Console.hpp"
#include <string>

namespace fpga::test {

    // 测试的应用程序
    // 参数 T 是测试实例的具体类型，需要继承自 TestInstance
    template <typename T>
        requires std::is_base_of_v<TestInstance, T>
    struct TestApplication {
    private:

        // 测试模块
        T bench;

    public:
        // 测试的入口函数
        void run(const char* input_file, const char* output_file, int cycles, int output_cycle_interval = 1000000) {

            // 输出测试的基本元数据
            Console::log("Test application start.");
            Console::log("Input file: " + std::string(input_file));
            Console::log("Output file: " + std::string(output_file));

            // 第一步，初始化测试模块
            Console::log("Step 1 : Initialize test bench.");
            try {
                bench.reset();
                bench.initialize(input_file, output_file);
                Console::log<fpga::test::Color::Green>("Initialize test bench successfully.");
            }
            catch (const std::exception& e) {
                Console::log<fpga::test::Color::Red>("Error: " + std::string(e.what()));
                return;
            }

            // 第二步，运行测试模块，运行 cycles 个周期
            Console::log("Step 2 : Run test bench.");
            int cycle = 0;
            try {
                while (cycle < cycles) {
                    if (cycle % output_cycle_interval == 0) {
                        Console::log<fpga::test::Color::Blue>("Cycle " + std::to_string(cycle) + " start.");
                    }
                    bench.tick();
                    cycle++;
                }
                Console::log<fpga::test::Color::Green>("Run test bench successfully.");
            }
            catch (const std::exception& e) {
                Console::log<fpga::test::Color::Red>("Error: " + std::string(e.what()));
                Console::log<fpga::test::Color::Red>("Error: Test bench run failed.");
                Console::log<fpga::test::Color::Red>("Error: Test application end.");
                return;
            }

            // 第三步，输出统计信息
            Console::log("Step 3 : Report test bench.");
            try {
                bench.report();
                Console::log<fpga::test::Color::Green>("Report test bench successfully.");
            }
            catch (const std::exception& e) {
                Console::log<fpga::test::Color::Red>("Error: " + std::string(e.what()));
                Console::log<fpga::test::Color::Red>("Error: Test bench report failed.");
                Console::log<fpga::test::Color::Red>("Error: Test application end.");
                return;
            }

            // 重置（关闭）测试模块
            bench.reset();

            // 测试结束
            Console::log("Test application end.");
        }
    };
}