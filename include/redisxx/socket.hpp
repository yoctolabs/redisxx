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

#include <redisxx/type_traits.hpp>

// note: user needs to explicitly include a socket wrapper to enable it

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

// tba: docs
template <typename SocketImpl>
typename std::enable_if<is_stream_socket<SocketImpl>::value, std::string>::type
execute(std::string const & host, std::uint16_t port, std::string const & request) {
	// create dedicated socket for this request
	SocketImpl socket{host}; // host contains filename
	// process request using this socket
	return priv::process(socket, request);
}

// tba: docs
template <typename SocketImpl>
typename std::enable_if<is_tcp_socket<SocketImpl>::value, std::string>::type
execute(std::string const & host, std::uint16_t port, std::string const & request) {
	// create dedicated socket for this request
	SocketImpl socket{host, port};
	// process request using this socket
	return priv::process(socket, request);
}

} // ::priv
} // ::redisxx

