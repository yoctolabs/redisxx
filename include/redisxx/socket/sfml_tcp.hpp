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
	
	public:
		SfmlTcpSocket(std::string const & host, std::uint16_t port)
			: socket{} {
			socket.setBlocking(true);
			auto status = socket.connect(host, port);
			if (status != sf::Socket::Done) {
				throw std::runtime_error{
					"Cannot connect to " + host + ":" + std::to_string(port)
				};
			}
		}

		void write(char const * data, std::size_t num_bytes) {
			auto status = socket.send(data, num_bytes);
			if (status != sf::Socket::Done) {
				throw std::runtime_error{"Cannot write bytes to socket"};
			}
		}

		std::size_t read(char* data, std::size_t num_bytes) {
			std::size_t received;
			auto status = socket.receive(data, num_bytes, received);
			if (status != sf::Socket::Done) {
				throw std::runtime_error{"Cannot read bytes from socket"};
			}
			return received;
		}
};

} // ::redisxx

