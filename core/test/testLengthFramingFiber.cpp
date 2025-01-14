#include <gtest/gtest.h>
#include <marlin/core/fibers/LengthFramingFiber.hpp>

using namespace marlin::core;

struct Source {
	int leftover(auto&& source, auto&& buf, SocketAddress addr) {
		return source.did_recv(*this, std::move(buf), addr);
	}
};

struct Terminal {
	Terminal(auto&&...) {}

	auto& i(auto&&) { return *this; }
	auto& o(auto&&) { return *this; }
	auto& is(auto& f) { return f; }
	auto& os(auto& f) { return f; }

	std::function<int(Buffer&&, uint64_t, SocketAddress)> did_recv_impl;
	int did_recv(auto&&, Buffer&& buf, uint64_t br, SocketAddress addr) {
		return did_recv_impl(std::move(buf), br, addr);
	}

	size_t c = 1;

	std::function<int(SocketAddress)> did_recv_frame_impl;
	int did_recv_frame(auto&& src, SocketAddress addr) {
		auto res = did_recv_frame_impl(addr);
		src.reset(++c);
		return res;
	}
};

TEST(LengthFramingFiber, Constructible) {
	Terminal t;
	LengthFramingFiber<Terminal&> f(std::forward_as_tuple(t));
}

TEST(LengthFramingFiber, SingleBuffer) {
	Source s;
	Terminal t;

	auto msg = Buffer(15).write_unsafe(0, (uint8_t const*)"abcdefghijklmno", 15);

	LengthFramingFiber<Terminal&> f(std::forward_as_tuple(t));
	f.reset(1);

	size_t bytes_calls = 0;
	t.did_recv_impl = [&](Buffer&& buf, uint64_t br, SocketAddress addr) {
		EXPECT_LT(bytes_calls, 5);
		if(bytes_calls == 0) {
			EXPECT_EQ(buf.size(), 1);
			EXPECT_EQ(std::memcmp(buf.data(), "a", 1), 0);
		} else if(bytes_calls == 1) {
			EXPECT_EQ(buf.size(), 2);
			EXPECT_EQ(std::memcmp(buf.data(), "bc", 2), 0);
		} else if(bytes_calls == 2) {
			EXPECT_EQ(buf.size(), 3);
			EXPECT_EQ(std::memcmp(buf.data(), "def", 3), 0);
		} else if(bytes_calls == 3) {
			EXPECT_EQ(buf.size(), 4);
			EXPECT_EQ(std::memcmp(buf.data(), "ghij", 4), 0);
		} else if(bytes_calls == 4) {
			EXPECT_EQ(buf.size(), 5);
			EXPECT_EQ(std::memcmp(buf.data(), "klmno", 5), 0);
		}
		EXPECT_EQ(br, 0);
		EXPECT_EQ(addr.to_string(), "192.168.0.1:8000");
		bytes_calls++;

		return 0;
	};

	size_t frame_calls = 0;
	t.did_recv_frame_impl = [&](SocketAddress addr) {
		EXPECT_LT(frame_calls, 5);
		EXPECT_EQ(frame_calls+1, bytes_calls);
		EXPECT_EQ(addr.to_string(), "192.168.0.1:8000");
		frame_calls++;

		return 0;
	};
	f.did_recv(s, std::move(msg), SocketAddress::from_string("192.168.0.1:8000"));
	EXPECT_EQ(bytes_calls, 5);
	EXPECT_EQ(frame_calls, 5);
}

