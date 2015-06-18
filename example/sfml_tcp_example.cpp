// needs to be linke against sfml-system and sfml-network
#include <iostream>
#include <redisxx/connection.hpp>
#include <redisxx/socket/sfml_tcp.hpp>
#include <redisxx/command.hpp>

int main() {
	redisxx::Connection<redisxx::SfmlTcpSocket> conn{"127.0.0.1", 6379u};

	auto reply1 = conn(redisxx::Command{"ping"});
	auto reply2 = conn(redisxx::Command{"info"});
	
	std::cout << reply1.get().getRaw() << std::endl;
	std::cout << reply2.get().getRaw() << std::endl;
}

