#include "case/basic/BasicSystem.hpp"
#include "case/basic/BasicAnalyzer.hpp"
#include "case/basic/BasicGenerator.hpp"
#include "test/TestApplication.hpp"

#include <iostream>

using namespace std;

static constexpr fpga::utils::FieldBundleProfile test_phv = {
	/* FIELD8_COUNT = */ 4,
	/* FIELD16_COUNT = */ 4,
	/* FIELD32_COUNT = */ 2
};

fpga::test::TestApplication<
	fpga::basic::BasicSystem<
		fpga::utils::Input<
			fpga::p4::Phv,
			fpga::basic::BasicGenerator<test_phv>,
			fpga::basic::BasicAnalyzer<test_phv>
		>,
		fpga::utils::Output<fpga::p4::Phv>,
		/* MAU_COUNT = */ 1
	>
> test_app;

int main()
{
	return 0;
}
