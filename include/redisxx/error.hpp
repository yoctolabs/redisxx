/** @file error.hpp
 *
 * RedisXX Error handling
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <stdexcept>
#include <string>
#include <vector>

#include <redisxx/common.hpp>

namespace redis {

class BaseError: public std::runtime_error {
	public:
		BaseError(std::string const & msg)
			: std::runtime_error{msg} {
		}
};

class ReplyError: public BaseError {
	public:
		ReplyError(std::vector<ReplyType> const & expected, ReplyType got)
			: BaseError{"Expected " + priv::to_string(expected) + " but got "
				+ priv::to_string(got)} {
		}
			
};

// tba: ConnectionError etc.

} // ::redis

