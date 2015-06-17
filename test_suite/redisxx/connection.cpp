#include <cstdint>
#include <string>
#include <cstring>
#include <boost/test/unit_test.hpp>

#include <redisxx/command.hpp>
#include <redisxx/connection.hpp>

class MockSocket {
	private:
		std::string buffer;
		std::size_t pos;
	
	public:
		MockSocket()
			: buffer{}
			, pos{0u} {
		}

		void write(char const * data, std::size_t num_bytes) {
			// determine reply
			std::string tmp{data, num_bytes};
			if (tmp == "*1\r\n$4\r\nPING\r\n") {
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

BOOST_AUTO_TEST_SUITE(redisxx_test_connection)

BOOST_AUTO_TEST_CASE(command_fetch_single_string) {
	MockSocket socket;
	auto string = redisxx::priv::fetch_string(socket, "*1\r\n$4\r\nPING\r\n");
	BOOST_CHECK_EQUAL(string, "+PONG\r\n");
}

BOOST_AUTO_TEST_CASE(command_connection_reply) {
	redisxx::Connection<MockSocket> conn; // note: host/port not required here
	redisxx::Command cmd{"PING"};
	auto reply = conn(cmd).get();
	BOOST_CHECK_EQUAL(reply, "+PONG\r\n");
}

BOOST_AUTO_TEST_SUITE_END()


