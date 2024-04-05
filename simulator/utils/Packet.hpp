#pragma once

namespace fpga::utils {

	// 一个默认的 Packet 类，每个 case 使用的实际 Packet 类都应该继承自这个类
	struct Packet {

		// packet 的 id（根据到达时间递增）
		const long long packet_id = -1;

		// packet 的到达时间（以硬件周期数为单位），由外部给定
		const long long arrive_time = -1;

		// packet 的 payload 长度（以硬件周期数为单位），由外部给定
		const int payload_cycles = -1;

		static long long packet_id_g;

		Packet() = default;
		
		Packet(long long arrive_time, int payload_cycles) : 
			arrive_time(arrive_time), payload_cycles(payload_cycles),
			packet_id(packet_id_g++) {}
	};

	long long Packet::packet_id_g = 0;

}