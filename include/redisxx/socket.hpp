/** @file socket.hpp
 *
 * RedisXX socket implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <string>

#if defined(REDISXX_SFML_SOCKET)
	#include <redisxx/socket/sfml_tcp.hpp>
#elif defined(REDISXX_BOOST_SOCKET)
	#include <redisxx/socket/boost_tcp.hpp>
#elif defined(REDISXX_UNIX_SOCKET)
	#include <redisxx/socket/boost_unix.hpp>
#endif

namespace redisxx {
namespace priv {

/// Process a request
/**
 *	This method processes the given request using the given socket. It reads
 *	the entire reply string and returns it. If no reply is available yet, this
 *	function will block until the reply is available.
 *
 *	@throw ConnectionError if an error occured
 *	@param socket Reference to a socket wrapper
 *	@param request RESP-compliant Request string
 *	@return Entire reply string
 */
template <typename SocketImpl>
std::string process(SocketImpl& socket, std::string const & request) {
	// write request
	socket.write(request.c_str(), request.size());
	// read chunks of the reply
	std::string buffer;
	char byte;
	socket.read_block(&byte, 1u);
	buffer += byte;
	std::size_t chunk_size = 256u;
	std::size_t pos = 1u;
	while (true) {
		// resize buffer
		buffer.resize(buffer.size() + chunk_size);
		// read chunk
		auto received = socket.read_some(&buffer[pos], chunk_size);
		pos += received;
		if (received < chunk_size) {
			// strip unused chars
			buffer.resize(buffer.size() - (chunk_size - received));
			break;
		}
	}
	return buffer;
}

} // ::priv
} // ::redisxx

