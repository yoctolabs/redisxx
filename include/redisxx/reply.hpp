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

#include <redisxx/error.hpp>

namespace redis {

class Reply {
	private:
		ReplyType type;
		
		bool status;
		std::string string; // for error and string reply
		std::vector<Reply> array; // for array reply
		
	public:
		/// tba: doc
		Reply()
			: type{ReplyType::Null} {
		}
		
		/// tba: doc + params
		Reply(bool status)
			: type{ReplyType::Status}
			, status{status} {
		}
		
		/// tba: doc + params
		Reply(std::string const & string, bool error=false)
			: string{string} {
			if (error) {
				type = ReplyType::Error;
			} else {
				type = ReplyType::String;
			}
		}
		
		/// tba: doc + params
		Reply(std::vector<Reply> const & array)
			: type{ReplyType::Array}
			, array{array} {
		}
		
		/// tba: doc + result
		ReplyType getType() const noexcept {
			return type;
		}
		
		// tba: doc + result + precondition + throw
		bool getStatus() const {
			if (type != ReplyType::Status) {
				throw ReplyError({ReplyType::Status}, type);
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
					throw ReplyError({ReplyType::String, ReplyType::Error}, type);
			}
		}

		// tba: doc + result + precondition + throw		
		std::vector<Reply> const & getArray() const {
			if (type != ReplyType::array) {
				throw ReplyError({ReplyType::Array}, type);
			}
			
			return array;
		}
		
		// tba: doc + param + result + precondition + throw
		Reply const & operator[](std::size_t index) const {
			if (type != ReplyType::array) {
				throw ReplyError({ReplyType::Array}, type);
			}
			
			return array.at(index);
		}
		
}

} // ::redis

