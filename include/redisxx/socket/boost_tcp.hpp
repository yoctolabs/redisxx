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
#include <boost/asio.hpp>

#include <redisxx/error.hpp>

namespace redisxx {

/// Socket wrapper using Boost Asio's TCP socket
class BoostTcpSocket {
	private:
		static boost::asio::io_service service;
		boost::asio::ip::tcp::socket socket;
		std::string const host;
		unsigned int port;
	
	public:
		BoostTcpSocket(std::string const & host, unsigned int port)
			: socket{BoostTcpSocket::service}
			, host{host}
			, port{port} {
		}
		
		BoostTcpSocket(BoostTcpSocket const & other)
			: socket{BoostTcpSocket::service}
			, host{other.host}
			, port{other.port} {
		}
		
		void open() {
			try {
				boost::asio::ip::tcp::resolver resolver{service};
				boost::asio::connect(socket, resolver.resolve({host, std::to_string(port)}));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), host, port};
			}
		}
		
		void close() {
			// tba
		}

		void write(char const * data, std::size_t num_bytes) {
			try {
				socket.send(boost::asio::buffer(data, num_bytes));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), host, port};
			}
		}

		void read_block(char* data, std::size_t num_bytes) {
			try {
				boost::asio::read(socket, (boost::asio::buffer(data, num_bytes)));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), host, port};
			}
		}

		std::size_t read_some(char* data, std::size_t num_bytes) {
			try {
				return socket.read_some(boost::asio::buffer(data, num_bytes));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), host, port};
			}
		}
};

// let's have one io_service for all of our socket
boost::asio::io_service BoostTcpSocket::service;

} // ::redisxx

