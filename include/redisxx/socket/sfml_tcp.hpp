/** @file sfml_tcp.hpp
 *
 * RedisXX Socket wrapper using SFML's TCP socket implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <cstdint>
#include <string>
#include <stdexcept>
#include <SFML/Network.hpp>

namespace redisxx {

/// Socket wrapper using SFML's TCP socket
class SfmlTcpSocket {
	private:
		sf::TcpSocket socket;
		std::string const host;
		unsigned int const port;
	
	public:
		SfmlTcpSocket(std::string const & host, unsigned int port)
			: socket{}
			, host{host}
			, port{port} {
			socket.setBlocking(true);
		}
		
		SfmlTcpSocket(SfmlTcpSocket const & other)
			: socket{}
			, host{other.host}
			, port{other.port} {
		}
		
		void open() {
			auto status = socket.connect(host, port);
			if (status != sf::Socket::Done) {
				throw std::runtime_error{
					"Cannot connect to " + host + ":" + std::to_string(port)
				};
			}
		}
		
		void close() {
			socket.disconnect();
		}

		void write(char const * data, std::size_t num_bytes) {
			auto status = socket.send(data, num_bytes);
			if (status != sf::Socket::Done) {
				throw std::runtime_error{"Cannot write bytes to socket"};
			}
		}

		void read_block(char* data, std::size_t num_bytes) {
			std::size_t unused;
			auto status = socket.receive(data, num_bytes, unused);
			if (status != sf::Socket::Done) {
				throw std::runtime_error{"Cannot read bytes from socket"};
			}
		}
		
		std::size_t read_some(char* data, std::size_t num_bytes) {
			std::size_t received;
			auto status = socket.receive(data, num_bytes, received);
			if (status != sf::Socket::Done) {
				throw std::runtime_error{"Cannot read bytes from socket"};
			}
			return received;
		}
};

} // ::redisxx

