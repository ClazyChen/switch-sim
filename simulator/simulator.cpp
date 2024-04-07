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

	fpga::utils::UInt<96> uint_num1;
	uint_num1.data[0] = 0xFFFF0000FFFF0000;
	uint_num1.data[1] = 0xFFFF0000;

	uint_num1.set_invalid_bits();
	uint_num1.clear_invalid_bits();

	fpga::utils::UInt<96> uint_num2;
	uint_num2.data[0] = 0x0FFFF0000FFFF000;
	uint_num2.data[1] = 0x0FFF000F;

	fpga::utils::UInt<96> uint_num3 = uint_num1 - 6;
	uint64_t x = 0xFFFF0000FFFF0000;

	//传入92位，显示为96位
	fpga::utils::UInt<92> uint_result_or = uint_num2 | uint_num1;
	fpga::utils::UInt<96> uint_result_and = uint_num2 & uint_num1;
	fpga::utils::UInt<96> uint_result_xor = uint_num2 ^ uint_num1;
	fpga::utils::UInt<96> uint_result_not = ~uint_num1;
	fpga::utils::UInt<96> uint_result_shift_left = uint_num1 << 2;
	fpga::utils::UInt<96> uint_result_shift_right = uint_num1 >> 4;
	bool uint_result_if_equal_1 = uint_num1 == uint_num2;
	bool uint_result_if_equal_2 = uint_num1 == uint_num3;
	auto uint_result_three_way_compare_greater = uint_num1 <=> uint_num2;
	auto uint_result_three_way_compare_less = uint_num2 <=> uint_num1;
	auto uint_result_three_way_compare_equal = uint_num3 <=> uint_num1;
	auto uint_result_add = uint_num1 + uint_num2;
	auto uint_result_add_uint64_left = x + uint_num1;
	auto uint_result_add_uint64_right = uint_num1 + x;
	auto uint_result_complement = -uint_num1;
	auto uint_result_sub = uint_num1 - uint_num3;


}
