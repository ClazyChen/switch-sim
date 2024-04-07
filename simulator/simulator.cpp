#include "cases/basic/BasicSystem.hpp"
#include "cases/basic/BasicAnalyzer.hpp"
#include "cases/basic/BasicGenerator.hpp"
#include "test/TestApplication.hpp"

#include <iostream>

using namespace std;

static constexpr fpga::utils::FieldBundleProfile test_phv = {
	/* FIELD8_COUNT = */ 4,
	/* FIELD16_COUNT = */ 4,
	/* FIELD32_COUNT = */ 2
};

fpga::test::TestApplication<
	fpga::cases::basic::BasicSystem<
		fpga::utils::Input<
			fpga::p4::Phv,
			fpga::cases::basic::BasicGenerator<test_phv>,
			fpga::cases::basic::BasicAnalyzer<test_phv>
		>,
		fpga::utils::Output<fpga::p4::Phv>,
		/* MAU_COUNT = */ 1
	>
> test_app;

int main()
{
	test_app.run("input.txt", "output.txt", 100);
	return 0;
}
