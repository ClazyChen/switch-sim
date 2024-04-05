#pragma once

#include "../../../utils.hpp"
#include "../../Pipeline.hpp"
#include "GetKeyConfig.hpp"

namespace fpga::p4::mat::getkey {
	template <size_t mau_id>
	struct Getkey : public Module {
		struct IO {
			fpga::p4::Pipeline pipe;
			Out<fpga::p4::LookupKey> lookup_key_out;
			Out<fpga::p4::GatewayKey> gateway_key_out;
		} io;

		void reset() {
			io.pipe.reset();
			io.gateway_key_out = nullptr;
			io.lookup_key_out = nullptr;
		}

		void update() {
			io.pipe.pass();
		}

		void run() {
			if (io.packet_in) {
				load_key<GetKeyConfig<mau_id>::Lookup>(io.lookup_key_out.get());
				load_key<GetKeyConfig<mau_id>::Gateway>(io.gateway_key_out.get());
            }
            else {
                io.lookup_key_out = nullptr;
                io.gateway_key_out = nullptr;
			}
		}

		<typename Cfg, typename Key>
		void load_key(Key& key) {
			load_field<8>(key.field8, Cfg::field8);
			load_field<16>(key.field16, Cfg::field16);
			load_field<32>(key.field32, Cfg::field32);
		}

		template <size_t width, typename A, typename Idx>
		void load_field(A& arr, Idx idx) {
			constexpr size_t count = arr.size();
			for (size_t i = 0; i < count; i++) {
				arr[i] = get_container<width>(idx(i));
			}
		}

		template <size_t width>
			requires (width == 8 || width == 16 || width == 32)
		constexpr uint64_t get_container(size_t container_id) const {
			const auto& phv = io.pipe.phv_in.get();
			if constexpr (fpga::Config::mat::getkey::any_connect) {
				if (container_id < fpga::Config::phv::field8_count) {
					return phv.field8[container_id];
				}
				container_id -= fpga::Config::phv::field8_count;
				if (container_id < fpga::Config::phv::field16_count) {
					return phv.field16[container_id];
				}
				container_id -= fpga::Config::phv::field16_count;
				if (container_id < fpga::Config::phv::field32_count) {
					return phv.field32[container_id];
				}
				return 0;
			}
			else {
				if constexpr (width == 8) {
					return phv.field8[container_id];
				}
				if constexpr (width == 16) {
					return phv.field16[container_id];
				}
				if constexpr (width == 32) {
					return phv.field32[container_id];
				}
			}
		}

	};
}