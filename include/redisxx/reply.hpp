/** @file reply.hpp
 *
 * RedisXX Reply implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <string>
#include <vector>

#include <redisxx/type_traits.hpp>

namespace redis {

// tba: error.hpp : Error, ReplyError, ConnectionError
// tba: unit test

enum class ReplyType {
	Status, Error, String, Array, Null
}

class Reply {
	private:
		ReplyType type;
		
		bool status;
		std::string string; // for error and string reply
		std::vector<Reply>; // for array reply
		
	public:
		/// tba: ctor(type, args) with args in {status, string, vector<Reply>}
		
		/// tba: doc + result
		ReplyType getType() const noexcept {
			return type;
		}
		
		// tba: doc + result + precondition + throw
		bool getStatus() const {
			if (type != ReplyType::Status) {
				throw ReplyError("Expected status reply", type);
			}
			
			return status;
		}
		
		// tba: doc + result + precondition + throw
		std::string const & getString() const {
			switch (type) {
				case ReplyType::string:
				case ReplyType::error:
					return string;
				
				default:
					throw ReplyError("Expected string or error reply", type);
			}
		}

		// tba: doc + result + precondition + throw		
		std::vector<Reply> const & getArray() const {
			if (type != ReplyType::array) {
				throw ReplyError("Expected array reply", type);
			}
			
			return array;
		}
		
		// tba: doc + param + result + precondition + throw
		Reply const & operator[](std::size_t index) const {
			if (type != ReplyType::array) {
				throw ReplyError("Expected array reply", type);
			}
			
			return array.at(index);
		}
		
}

} // ::redis

