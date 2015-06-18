/** @file boost_tcp.hpp
 *
 * RedisXX Socket wrapper using Boost for Unix Domain Socket implementation
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

#if not defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
#error UnixDomainSocket is not available through Boost.
#endif

namespace redisxx {

/// Socket wrapper using Boost for Unix Domain Socket
class BoostUnixSocket {
	private:
		static boost::asio::io_service service;
		boost::asio::local::stream_protocol::socket socket;
		std::string const filename;
	
	public:
		BoostUnixSocket(std::string const & filename)
			: socket{BoostUnixSocket::service}
			, filename{filename} {
			try {
				socket.connect(boost::asio::local::stream_protocol::endpoint(filename));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), filename};
			}
		}
		
		void write(char const * data, std::size_t num_bytes) {
			try {
				socket.send(boost::asio::buffer(data, num_bytes));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), filename};
			}
		}

		void read_block(char* data, std::size_t num_bytes) {
			try {
				boost::asio::read(socket, (boost::asio::buffer(data, num_bytes)));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), filename};
			}
		}

		std::size_t read_some(char* data, std::size_t num_bytes) {
			try {
				return socket.read_some(boost::asio::buffer(data, num_bytes));
			} catch (boost::system::system_error const & e) {
				// wrap to general exception
				throw ConnectionError{e.what(), filename};
			}
		}
};

// let's have one io_service for all of our socket
boost::asio::io_service BoostUnixSocket::service;

} // ::redisxx

