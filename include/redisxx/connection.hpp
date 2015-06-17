/** @file connection.hpp
 *
 * RedisXX Connection implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <future>

#include <redisxx/socket.hpp>
//#include <redisxx/reply.hpp>

namespace redisxx {
namespace priv {

template <typename SocketImpl>
std::string fetch_string(SocketImpl& socket, std::string const & string) {
	// write request
	socket.write(string.c_str(), string.size());
	// read reply
	// tba: read entire string and use it to construct reply
	return "";
}

} // ::priv


// ----------------------------------------------------------------------------

// dummy
using Reply = std::string;



template <typename SocketImpl = DefaultSocket>
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
				return Reply{priv::fetch_string(socket, string)};
			});
		}
};

} // ::redisxx

