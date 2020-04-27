/*! \file StreamPacket.hpp
*/

#ifndef MARLIN_STREAM_STREAMPACKET_HPP
#define MARLIN_STREAM_STREAMPACKET_HPP

#include <marlin/core/Buffer.hpp>

#include <cstring>
#include <arpa/inet.h>

namespace marlin {
namespace stream {

//! Buffer implementation class to read the packets recieved on StreamTransport
/*!

\verbatim

 0               1               2               3
 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
|   version(1)  |  type/fin(1)  |                               |
------------------------------------------------------------------
|                         src_conn_id(4)                        |
------------------------------------------------------------------
|                         dst_conn_id(4)                        |
------------------------------------------------------------------
|      stream_id/size(2)        |                               |
------------------------------------------------------------------
|                                                               |
---                    Packet Number (8)                      ----
|                                                               |
------------------------------------------------------------------
|                                                               |
---                 Data offset in stream (8)                 ----
|                                                               |
-----------------------------------------------------------------
|                              ...                              |
-----------------------------------------------------------------
|                           Data (N)                            |
-----------------------------------------------------------------
|                              ...                              |
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	Packet breakup:
	0	:	1	:	version
	1	:	1	:	type_flag / fin flag in case of data packet
	2	:	4	:	src_conn_id
	6	: 	4	:	dst_conn_id
	10	:	2	: 	stream_id for data packets, size for (ack?)
	12	:	8	:	packet_number (valid for data and ack packets)
	20	: 	8	:	offset of data in stream (only valid for data packets)


	Type-flags:
	0/1	:	data/data+fin
	2	:	ack
	3	:	dial
	4	:	dial_conf
	5	:	conf
	6	:	reset

\endverbatim
*/

struct StreamPacket: public core::Buffer {
	uint8_t version() const {
		// FIXME: Check
		return read_uint8(0).value();
	}

	uint8_t message() const {
		// FIXME: Check
		return read_uint8(1).value();
	}

	bool is_fin_set() const {
		return message() == 1;
	}

	uint16_t stream_id() const {
		// FIXME: Check
		return read_uint16(10).value();
	}

	uint64_t packet_number() const {
		// FIXME: Check
		return read_uint64(12).value();
	}

	uint64_t offset() const {
		// FIXME: Check
		return read_uint64(20).value();
	}

	uint16_t length() const {
		// FIXME: Check
		return read_uint16(28).value();
	}
};

} // namespace stream
} // namespace marlin

#endif // MARLIN_STREAM_STREAMPACKET_HPP
