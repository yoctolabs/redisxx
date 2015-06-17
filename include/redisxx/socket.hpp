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
#endif

namespace redisxx {
namespace priv {

template <typename SocketImpl>
std::string process(SocketImpl& socket, std::string const & string) {
	// write request
	socket.write(string.c_str(), string.size());
	// read chunks of the reply
	std::string buffer;
	std::size_t chunk_size = 256u;
	std::size_t pos = 0u;
	while (true) {
		// resize buffer
		buffer.resize(buffer.size() + chunk_size);
		// read chunk
		auto received = socket.read(&buffer[pos], chunk_size);
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

