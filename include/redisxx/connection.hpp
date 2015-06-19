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


// tba: docs
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
		// tba: docs
		Connection(std::string const & host, std::uint16_t port=0u)
			: host{host}
			, port{port} {
		}
		
		// tba: docs
		template <typename Request>
		std::future<Reply> operator()(Request const & request) {
			auto string = *request;
			return std::async(std::launch::async, [&, string]() {
				return Reply{priv::execute<SocketImpl>(host, port, string)};
			});
		}
};


} // ::redisxx

