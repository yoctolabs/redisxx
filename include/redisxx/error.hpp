/** @file error.hpp
 *
 * RedisXX Exception classes
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <stdexcept>
#include <string>
#include <cstdint>

namespace redisxx {

class ConnectionError: public std::runtime_error {
	private:
		std::string const host;
		unsigned int const port;
		
	public:
		ConnectionError(std::string const & msg, std::string const & host, unsigned int port)
			: std::runtime_error{msg}
			, host{host}
			, port{port} {
		}

		inline std::string const & getHost() const {
			return host;
		}

		inline unsigned int getPort() const {
			return port;
		}
};

} // ::redisxx
