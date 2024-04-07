#include "utils/Wire.hpp"
#include "p4/Pipeline.hpp"
#include "utils/UInt.hpp"

#include <iostream>

using namespace std;

int main()
{
	/*fpga::utils::Wire<int> x;
	x = 5;
	cout << x << endl;
	return 0;*/
	//只有判断是否相等，没有判断是否不等
	//data[-1]的注释
	//与，或没有与64位数操作的情况
	//Index运算取溢出位不够严谨

	fpga::utils::UInt<96> uint_num1;
	uint_num1.data[0] = 0xFFFF0000FFFF0000;
	uint_num1.data[1] = 0xFFFF0000;

	uint_num1.set_invalid_bits();
	uint_num1.clear_invalid_bits();

	fpga::utils::UInt<96> uint_num2;
	uint_num2.data[0] = 0x0FFFF0000FFFF000;
	uint_num2.data[1] = 0x0FFF000F;

	fpga::utils::UInt<96> uint_num3 = uint_num1;
	uint64_t x = 0xFFFF0000FFFF0000;
	uint64_t y = 0x0000000000000001;

	fpga::utils::UInt<96> uint_num4;
	uint_num4.data[0] = 0x0000000000000001;
	uint_num4.data[1] = 0x00000000;
	fpga::utils::UInt<96> uint_num5;
	uint_num5.data[0] = 0x0000000000000001;
	uint_num5.data[1] = 0xF0000000;

	fpga::utils::UInt<64> uint_num6;
	uint_num6.data[0] = 0xFFFF0000FFFF0000;


	//传入92位，显示为96位
	fpga::utils::UInt<68> uint_result_or = uint_num2 | uint_num1;
	fpga::utils::UInt<96> uint_result_and = uint_num6 & uint_num1;
	fpga::utils::UInt<96> uint_result_xor = uint_num2 ^ uint_num1;
	fpga::utils::UInt<96> uint_result_not = ~uint_num1;
	fpga::utils::UInt<96> uint_result_shift_left = uint_num1 << 2;
	fpga::utils::UInt<96> uint_result_shift_right = uint_num1 >> 4;
	bool uint_result_if_equal_1 = uint_num1 == uint_num2;
	bool uint_result_if_equal_2 = uint_num1 == uint_num3;
	auto uint_result_three_way_compare_greater = uint_num1 <=> uint_num2;
	auto uint_result_three_way_compare_less = uint_num2 <=> uint_num1;
	auto uint_result_three_way_compare_equal = uint_num3 <=> uint_num1;
	auto uint_result_add = uint_num4 + uint_num5;
	auto uint_result_add_uint64_left = x + uint_num1;
	auto uint_result_add_uint64_right = uint_num1 + x;
	//auto uint_result_complement = -uint_num1;
	auto uint_result_sub = uint_num4 - uint_num5;
	auto uint_result_sub_uint64_left = y - uint_num5;
	auto uint_result_sub_uint64_right = uint_num5 - y;
	//uint_num1 |= uint_num2;
	//uint_num1 &= uint_num2;
	//uint_num1 ^= uint_num2;
	//uint_num1 <<= 2;
	//uint_num1 >>= 4;
	//uint_num4 += uint_num5;

	/*uint_num4 += y;
	uint_num4 -= y;
	uint_num5 -= uint_num4;*/

	auto uint_result_index1 = uint_num1(0);
	auto uint_result_index2 = uint_num1(16);
	auto uint_result_index3 = uint_num1(17);
	auto uint_result_index4 = uint_num1(95);
	auto uint_result_index5 = uint_num1(96);

	auto uint_result_connect = uint_num1 && uint_num2;



}
