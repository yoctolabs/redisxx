#include <string>
#include <cstring>
#include <boost/test/unit_test.hpp>

#include <redisxx/socket.hpp>

struct MockSocket {
	std::string buffer;
	std::size_t pos;
	
	MockSocket()
		: buffer{}
		, pos{0} {
	}

	void write(char const * data, std::size_t num_bytes) {
		// determine reply
		std::string tmp{data, num_bytes};
		// note: these aren't valid redis requests, but this is only a mock ;)
		if (tmp == "status") {
			buffer = "+OK\r\n";
			
		} else if (tmp == "number") {
			buffer = ":124\r\n";
			
		} else if (tmp == "bulk") {
			buffer = "$14\r\nThis is a test\r\n";
			
		} else if (tmp == "array") {
			buffer = "*5\r\n$10\r\nhello world\r\n:15634\r\n+OK\r\n-No\r\n";
			
		} else if (tmp == "huge") {
			buffer = "$1500\r\n";
			for (auto i = 0u; i < 100; ++i) {
				buffer += "hello world!!! ";
			}
			buffer += "\r\n";
			
		} else {
			buffer = "-Unknown Command\r\n";
		}
	}

	std::size_t read(char* data, std::size_t num_bytes) {
		// determine number of received bytes
		std::size_t received = num_bytes;
		if (pos + num_bytes >= buffer.size()) {
			received = buffer.size() - pos;
		}
		// copy string
		std::strncpy(data, buffer.data() + pos, received);
		pos += received;
		return received;
	}
};

// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_SUITE(redisxx_test_socket)

BOOST_AUTO_TEST_CASE(socket_test_status_string) {
	MockSocket socket;
	auto out = redisxx::priv::process(socket, "status");
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_test_number_string) {
	MockSocket socket;
	auto out = redisxx::priv::process(socket, "number");
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_test_bulk_string) {
	MockSocket socket;
	auto out = redisxx::priv::process(socket, "bulk");
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_test_array_string) {
	MockSocket socket;
	auto out = redisxx::priv::process(socket, "array");
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_test_huge_bulk_string) {
	MockSocket socket;
	auto out = redisxx::priv::process(socket, "huge");
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_test_error_string) {
	MockSocket socket;
	auto out = redisxx::priv::process(socket, "foo bar");
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_SUITE_END()

