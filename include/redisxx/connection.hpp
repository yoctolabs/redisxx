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

#include <redisxx/socket.hpp>
//#include <redisxx/reply.hpp>

namespace redisxx {

// tba: replace with actual reply implementation
using Reply = std::string;


/// Connection based on the given SocketImpl type
/**
 *	This class manages all connections to the specified remote host or local
 *	stream using the given Socket implementation.
 *	The default socket implementation depends on which socket implementation
 *	is included.
 */
#if defined(REDISXX_UNIX_SOCKET)
template <typename SocketImpl = BoostUnixSocket>
#elif defined(REDISXX_BOOST_SOCKET)
template <typename SocketImpl = BoostTcpSocket>
#elif defined(REDISXX_SFML_SOCKET)
template <typename SocketImpl = SfmlTcpSocket>
#elif defined(REDISXX_SDLNET_SOCKET)
template <typename SocketImpl = SdlNetTcpSocket>
#else
template <typename SocketImpl>
#endif
class Connection {
	private:
		std::string const host;
		std::uint16_t const port;
		
	public:
		/// Create a new connection to the given remote host or local stream
		/**
		 *	If using a TCP Socket implementation, the port number is required
		 *	to be a valid port number. Else, when using a Streaming Socket
		 *	implementation, the port is not used here. In this case the host
		 *	parameter is used to describe which local stream should be used
		 *	(e.g. by specifying its filename).
		 *
		 *	@param host remote host's name OR local stream's filename
		 *	@param host remote host's port number OR not used
		 */
		Connection(std::string const & host, std::uint16_t port=0u)
			: host{host}
			, port{port} {
		}
		
		/// Execute the given request
		/**
		 *	This is used to execute the given request. The actual query is done
		 *	asynchronously, so a `std::future<>` is returned. A request needs
		 *	to be a `redisxx::Command` or `redisxx::CommandList` in order to
		 *	guarantee, that the actual request is RESP-compliant.
		 *	If the request needs to be synchronous, the returning future can
		 *	be forced to block by using `get()` until the query is done.
		 *	Each query will open, use and close the socket each time.
		 *
		 *	Example usage:
		 *	@code
		 *		// example 1: asynchronous tcp connection
		 *		redisxx::Connection<MyTcpSocket> conn1{"localhost", 6379};
		 *		auto async_reply = conn1(redisxx::Command{"PING"});
		 *		
		 *		// example 2: synchronous stream socket
		 *		redisxx::Connection<MyStreamSocket> conn2{"/tmp/redis.sock"};
		 *		auto sync_reply = conn2(redisxx::Command{"INFO"}).get();
		 *	@endcode
		 */
		template <typename Request>
		std::future<Reply> operator()(Request const & request) {
			auto string = *request;
			return std::async(std::launch::async, [&, string]() {
				return Reply{priv::execute_request<SocketImpl>(host, port, string)};
			});
		}
};


} // ::redisxx

