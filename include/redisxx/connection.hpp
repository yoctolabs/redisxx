/** @file connection.hpp
 *
 * RedisXX Connection implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <string>
#include <future>

#include <redisxx/type_traits.hpp>
#include <redisxx/socket.hpp>
//#include <redisxx/reply.hpp>

namespace redisxx {

// dummy
class Reply {
	private:
		std::string raw;

	public:
		Reply(std::string const & raw)
			: raw{raw} {
		}
		
		inline std::string const & getRaw() const {
			return raw;
		}
};
// end dummy

namespace priv {

template <typename SocketImpl>
typename std::enable_if<is_stream_socket<SocketImpl>::value, Reply>::type
execute(std::string const & host, std::uint16_t port, std::string const & request) {
	// create dedicated socket for this request
	SocketImpl socket{host}; // host contains filename
	// process request using this socket
	return {priv::process(socket, request)};
}

template <typename SocketImpl>
typename std::enable_if<is_tcp_socket<SocketImpl>::value, Reply>::type
execute(std::string const & host, std::uint16_t port, std::string const & request) {
	// create dedicated socket for this request
	SocketImpl socket{host, port};
	// process request using this socket
	return {priv::process(socket, request)};
}

} //::priv


#if defined(REDISXX_UNIX_SOCKET)
template <typename SocketImpl = BoostUnixSocket>
#elif defined(REDISXX_BOOST_SOCKET)
template <typename SocketImpl = BoostTcpSocket>
#elif defined(REDISXX_SFML_SOCKET)
template <typename SocketImpl = SfmlTcpSocket>
#else
template <typename SocketImpl>
#endif
class Connection {
	private:
		std::string const host;
		std::uint16_t const port;
		
	public:
		Connection(std::string const & host, std::uint16_t port=0u)
			: host{host}
			, port{port} {
		}
		
		template <typename Request>
		std::future<Reply> operator()(Request const & request) {
			auto string = *request;
			return std::async(std::launch::async, [&, string]() {
				return priv::execute<SocketImpl>(host, port, string);
			});
		}
};


} // ::redisxx

