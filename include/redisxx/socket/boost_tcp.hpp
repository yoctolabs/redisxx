/** @file boost_tcp.hpp
 *
 * RedisXX Socket wrapper using Boost.Asio's TCP socket implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <cstdint>
#include <string>
#include <stdexcept>
#include <boost/asio.hpp>

namespace redisxx {

/// Socket wrapper using Boost Asio's TCP socket
class BoostTcpSocket {
	private:
		boost::asio::io_service service;
		boost::asio::ip::tcp::socket socket;
	
	public:
		BoostTcpSocket(std::string const & host, std::uint16_t port)
			: service{}
			, socket{service} {
			try {
				boost::asio::ip::tcp::resolver resolver{service};
				boost::asio::connect(socket, resolver.resolve({host, std::to_string(port)}));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw std::runtime_error{e.what()};
			}
		}

		void write(char const * data, std::size_t num_bytes) {
			try {
				socket.send(boost::asio::buffer(data, num_bytes));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw std::runtime_error{e.what()};
			}
		}

		std::size_t read(char* data, std::size_t num_bytes) {
			try {
				return socket.read_some(boost::asio::buffer(data, num_bytes));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw std::runtime_error{e.what()};
			}
		}
};

} // ::redisxx

