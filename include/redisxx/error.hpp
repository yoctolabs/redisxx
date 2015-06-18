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
		std::string const host, filename;
		unsigned int const port;
		
	public:
		ConnectionError(std::string const & msg, std::string const & host, unsigned int port)
			: std::runtime_error{msg}
			, host{host}
			, filename{""}
			, port{port} {
		}
		
		ConnectionError(std::string const & msg, std::string const & filename)
			: std::runtime_error{msg}
			, host{}
			, filename{filename}
			, port{0u} {
		}

		inline std::string const & getHost() const {
			return host;
		}
		
		inline std::string const & getFilename() const {
			return filename;
		}

		inline unsigned int getPort() const {
			return port;
		}
};

} // ::redisxx
