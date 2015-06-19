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

// tba: docs
class ConnectionError: public std::runtime_error {
	private:
		std::string const host, filename;
		unsigned int const port;
		
	public:
		// tba: docs
		ConnectionError(std::string const & msg, std::string const & host, unsigned int port)
			: std::runtime_error{msg}
			, host{host}
			, filename{""}
			, port{port} {
		}
		
		// tba: docs
		ConnectionError(std::string const & msg, std::string const & filename)
			: std::runtime_error{msg}
			, host{}
			, filename{filename}
			, port{0u} {
		}

		// tba: docs
		inline std::string const & getHost() const {
			return host;
		}
		
		// tba: docs
		inline std::string const & getFilename() const {
			return filename;
		}

		// tba: docs
		inline unsigned int getPort() const {
			return port;
		}
};

} // ::redisxx
