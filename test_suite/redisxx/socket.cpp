#include <cstdint>
#include <string>
#include <cstring>
#include <boost/test/unit_test.hpp>

#include <redisxx/socket.hpp>

struct MockSocket {
	std::string buffer;
	std::size_t pos;
	
	MockSocket()
		: buffer{}
		, pos{0u} {
	}

	void write(char const * data, std::size_t num_bytes) {
		// determine reply
		std::string tmp{data, num_bytes};
		if (tmp == "*1\r\n$1\r\nPING\r\n") {
			buffer += "+PONG\r\n";
		} else {
			buffer += "-Unknown Command\r\n";
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

BOOST_AUTO_TEST_CASE(socket_fetch_status) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "+Hello World\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_long_status) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "+This is a very very loooong string\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_empty_status) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "+\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_multiple_status) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "+OK\r\n+Later this\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, "+OK\r\n");

	out.clear();
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, "+Later this\r\n");
}

BOOST_AUTO_TEST_CASE(socket_fetch_error) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "-Don't know what I'm doing\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_integer) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = ":13945\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_mixed_strings) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "-Unknown Command\r\n+OK\r\n:1203\r\n+OK\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, "-Unknown Command\r\n");

	out.clear();
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, "+OK\r\n");

	out.clear();
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, ":1203\r\n");

	out.clear();
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, "+OK\r\n");
}

BOOST_AUTO_TEST_CASE(socket_fetch_normal_bulk) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "$3\r\nfoo\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_long_bulk) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "$150\r\n";
	for (auto i = 0u; i < 10; ++i) {
		socket.buffer += "hello world!!! ";
	}
	socket.buffer += "\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_empty_bulk) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "$0\r\n\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_multiple_bulks) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "$3\r\nfoo\r\n$5\r\nhello\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, "$3\r\nfoo\r\n");
	
	out.clear();
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, "$5\r\nhello\r\n");
}

BOOST_AUTO_TEST_CASE(socket_fetch_null) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "$-1\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_CASE(socket_fetch_array_small) {
	MockSocket socket;
	std::string out;
	
	socket.buffer = "*2\r\n+OK\r\n-No\r\n";
	redisxx::priv::fetch_unknown(socket, out);
	BOOST_CHECK_EQUAL(out, socket.buffer);
}

BOOST_AUTO_TEST_SUITE_END()

