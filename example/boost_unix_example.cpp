// needs to be linke against boost_system, boost_thread and with -pthread
#include <iostream>
#include <redisxx/connection.hpp>
#include <redisxx/command.hpp>
#include <redisxx/socket/boost_unix.hpp> // enables UnixDomainSocket

int main() {
	redisxx::Connection<redisxx::BoostUnixSocket> conn{"/tmp/redis.sock"};

	auto reply1 = conn(redisxx::Command{"ping"});
	auto reply2 = conn(redisxx::Command{"info"});
	
	std::cout << reply1.get().getRaw() << std::endl;
	std::cout << reply2.get().getRaw() << std::endl;
}

