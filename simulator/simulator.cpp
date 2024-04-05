#include "utils/Wire.hpp"
#include "p4/Pipeline.hpp"

#include <iostream>

using namespace std;

int main()
{
	fpga::utils::Wire<int> x;
	x = 5;
	cout << x << endl;
	return 0;
}
