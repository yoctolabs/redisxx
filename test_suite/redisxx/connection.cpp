#include <string>
#include <cstring>
#include <boost/test/unit_test.hpp>

#include <redisxx/connection.hpp>
#include <redisxx/command.hpp>
#include <redisxx/socket/boost_unix.hpp>
#include <redisxx/socket/boost_tcp.hpp>
#include <redisxx/socket/sfml_tcp.hpp>

struct MockSocket {
	std::string buffer;
	std::size_t pos;
	
	// host/port ignored here
	MockSocket(std::string const &, std::uint16_t) {
		buffer.clear();
		pos = 0u;
	}
	
	void write(char const * data, std::size_t num_bytes) {
		// determine reply
		std::string tmp{data, num_bytes};
		// note: these aren't valid redis requests, but this is only a mock ;)
		if (tmp == "*1\r\n$4\r\nPING\r\n") {
			buffer = "+PONG\r\n";
			
		} else {
			buffer = "-Unknown Command\r\n";
		}
	}

	void read_block(char* data, std::size_t num_bytes) {
		if (pos + num_bytes >= buffer.size()) {
			throw "This should not happen with the mock!";
		}
		std::strncpy(data, buffer.data() + pos, num_bytes);
		pos += num_bytes;
	}

	std::size_t read_some(char* data, std::size_t num_bytes) {
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

BOOST_AUTO_TEST_SUITE(redisxx_test_connection)

BOOST_AUTO_TEST_CASE(connection_boost_unix_compile_test) {
	try {
		// this MIGHT throw, but that's not what we're testing here
		redisxx::Connection<redisxx::BoostUnixSocket> socket{"/tmp/redis.sock"};
	} catch (redisxx::ConnectionError const &) {
	}
}

BOOST_AUTO_TEST_CASE(connection_boost_tcp_compile_test) {
	try {
		// this MIGHT throw, but that's not what we're testing here
		redisxx::Connection<redisxx::BoostTcpSocket> socket{"localhost", 6379};
	} catch (redisxx::ConnectionError const &) {
	}
}

BOOST_AUTO_TEST_CASE(connection_sfml_tcp_compile_test) {
	try {
		// this MIGHT throw, but that's not what we're testing here
		redisxx::Connection<redisxx::SfmlTcpSocket> socket{"localhost", 6379};
	} catch (redisxx::ConnectionError const &) {
	}
}

BOOST_AUTO_TEST_CASE(connection_socket_ping) {
	redisxx::Connection<MockSocket> conn{"", 0}; // host/port ignored by mock
	auto reply = conn(redisxx::Command("PING")).get();
	BOOST_CHECK_EQUAL(reply.getRaw(), "+PONG\r\n");
}

BOOST_AUTO_TEST_SUITE_END()

