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


// note: default template parameter isn't working well with #ifdef AND template specialization .. I'm on it! ;)

template <typename SocketImpl>
class Connection {
	private:
		std::string const host;
		std::uint16_t const port;
		
	public:
		Connection(std::string const & host, std::uint16_t port)
			: host{host}
			, port{port} {
		}
		
		template <typename Request>
		std::future<Reply> operator()(Request const & request) {
			auto string = *request;
			// note: catch string by value because it's local!
			return std::async(std::launch::async, [&, string]() {
				// create dedicated socket for this request
				SocketImpl socket{host, port};
				return Reply{priv::process(socket, string)};
			});
		}
};


#if defined(REDISXX_UNIX_SOCKET)
// Specialization for UnixDomainSocket
template <>
class Connection<BoostUnixSocket> {
	private:
		std::string const filename;
		
	public:
		Connection(std::string const & filename)
			: filename{filename} {
		}
		
		template <typename Request>
		std::future<Reply> operator()(Request const & request) {
			auto string = *request;
			// note: catch string by value because it's local!
			return std::async(std::launch::async, [&, string]() {
				// create dedicated socket for this request
				BoostUnixSocket socket{filename};
				return Reply{priv::process(socket, string)};
			});
		}
};
#endif


} // ::redisxx

