#include "test/TestApplication.hpp"
#include "p4/MatSystem.hpp"
fpga::test::TestApplication<
    fpga::p4::MatSystem<1, 1, 1>
> app;

// 运行方式：
// pipeline.exe <input_file> <output_file> <cycles>
int main(int argc, char* argv[]) {

    // 保证输入的是 3 个参数
    /*if (argc != 4) {
        std::cout << "Usage: " << argv[0] << " <input_file> <output_file> <cycles>" << std::endl;
        return 1;
    }*/

    // 解析输入、输出文件名为 const char*
    const char* input_file = "in.txt";
    const char* output_file = "out.txt";

    // 解析 cycles 为 int
    int cycles = 8;

    // 执行测试
    app.run(input_file, output_file, cycles);

    return 0;
}
