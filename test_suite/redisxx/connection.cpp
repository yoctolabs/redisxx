#include <string>
#include <cstring>
#include <boost/test/unit_test.hpp>

#include <redisxx/connection.hpp>
#include <redisxx/socket/boost_unix.hpp>
#include <redisxx/socket/boost_tcp.hpp>
#include <redisxx/socket/sfml_tcp.hpp>
#include <redisxx/socket/sdlnet_tcp.hpp>

BOOST_AUTO_TEST_SUITE(redisxx_test_connection)

BOOST_AUTO_TEST_CASE(connection_boost_unix_compile_test) {
	try {
		// this MIGHT throw, but we're testing compilation here
		redisxx::Connection<redisxx::BoostUnixSocket> socket{"/tmp/redis.sock"};
	} catch (redisxx::ConnectionError const &) {
	}
}

BOOST_AUTO_TEST_CASE(connection_boost_tcp_compile_test) {
	try {
		// this MIGHT throw, but we're testing compilation here
		redisxx::Connection<redisxx::BoostTcpSocket> socket{"localhost", 6379};
	} catch (redisxx::ConnectionError const &) {
	}
}

BOOST_AUTO_TEST_CASE(connection_sfml_tcp_compile_test) {
	try {
		// this MIGHT throw, but we're testing compilation here
		redisxx::Connection<redisxx::SfmlTcpSocket> socket{"localhost", 6379};
	} catch (redisxx::ConnectionError const &) {
	}
}

BOOST_AUTO_TEST_CASE(connection_sdlnet_tcp_compile_test) {
	try {
		// this MIGHT throw, but we're testing compilation here
		redisxx::Connection<redisxx::SdlNetTcpSocket> socket{"localhost", 6379};
	} catch (redisxx::ConnectionError const &) {
	}
}

// tba: test more !!

BOOST_AUTO_TEST_SUITE_END()

