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

// dummy
class Reply {
	private:
		std::string s;

	public:
		Reply(std::string const & raw)
			: s{raw.substr(1u, raw.size() - 3u)} {
		}
		
		inline std::string const & getString() const {
			return s;
		}
};
// end dummy


#if defined(REDISXX_SFML_SOCKET)
template <typename SocketImpl = SfmlTcpSocket>
#elif defined(REDISXX_BOOST_SOCKET)
template <typename SocketImpl = BoostTcpSocket>
#else
template <typename SocketImpl>
#endif
class Connection {
	private:
		SocketImpl socket;
		
	public:
		template <typename ...Args>
		Connection(Args&& ...args)
			: socket{std::forward<Args>(args)...} {
		}
		
		template <typename Request>
		std::future<Reply> operator()(Request const & request) {
			auto string = *request;
			// note: catch string by value because it's local!
			return std::async(std::launch::async, [&, string]() {
				return Reply{priv::process(socket, string)};
			});
		}
};

} // ::redisxx

