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
		/// Construct a new null-reply
		/**
		 *	Creates a new reply with null-content.
		 */
		Reply()
			: type{ReplyType::Null} {
		}
		
		/// Construct a new status-reply
		/**
		 *	Creates a new reply with the given status.
		 *	@param status Determines whether previous query returned "OK" or not
		 */
		Reply(bool status)
			: type{ReplyType::Status}
			, status{status} {
		}
		
		/// Construct a new string- or error-reply
		/**
		 *	Creates a new reply with the given string. This is used for string-
		 *	replies as well as error-replies.
		 *	@param string String-content of the reply
		 *	@param error Determines whether the string is an error string or not
		 */
		Reply(std::string const & string, bool error=false)
			: type{error ? ReplyType::Error : ReplyType::String}
			, string{string} {
		}
		
		/// Construct a new array-reply
		/**
		 *	Creates a new reply with an array of other replies.
		 *	@param array Reply-array containing sub-replies
		 */
		Reply(std::vector<Reply> const & array)
			: type{ReplyType::Array}
			, array{array} {
		}
		
		/// Query the reply type
		/**
		 *	Used to query the actual reply type for e.g. error handling.
		 *	@return Reply type of the reply object
		 */
		ReplyType getType() const noexcept {
			return type;
		}
		
		/// Query the size of the reply-array inside the reply
		/**
		 *	Used to query the size of the reply-array of an array-reply.
		 *	@pre getType() == ReplyType::Array
		 *	@return Size of the reply-array
		 *	@throw ReplyError If precondition is violated
		 */
		std::size_t getSize() const {
			if (type != ReplyType::Array) {
				throw ReplyError({ReplyType::Array}, type);
			}
			
			return array.size();
		}
		
		/// Query the state of the reply
		/**
		 *	Used to query the boolean state of a state-reply.
		 *	@pre getType() == ReplyType::Status
		 *	@return Boolean status of the reply
		 *	@throw ReplyError If precondition is violated
		 */
		bool getStatus() const {
			if (type != ReplyType::Status) {
				throw ReplyError({ReplyType::Status}, type);
			}
			
			return status;
		}
		
		/// Query the string of the reply
		/**
		 *	Used to query the string content of a string-reply.
		 *	@pre getType() == ReplyType::String || getType() == ReplyType::Error
		 *	@return String content of the reply
		 *	@throw ReplyError If precondition is violated
		 */
		std::string const & getString() const {
			switch (type) {
				case ReplyType::string:
				case ReplyType::error:
					return string;
				
				default:
					throw ReplyError({ReplyType::String, ReplyType::Error}, type);
			}
		}

		/// Query the reply-array inside the reply
		/**
		 *	Used to query the reply-array of an array-reply.
		 *	@pre getType() == ReplyType::Array
		 *	@return Reply-Array of the reply
		 *	@throw ReplyError If precondition is violated
		 */
		std::vector<Reply> const & getArray() const {
			if (type != ReplyType::array) {
				throw ReplyError({ReplyType::Array}, type);
			}
			
			return array;
		}
		
		/// Const-access an element in the reply-array inside the reply
		/**
		 *	Used to const-access an element inside the reply-array of an
		 *	array-reply.
		 *	@pre getType() == ReplyType::Array
		 *	@pre getSize() > index
		 *	@return Reply-Array of the reply
		 *	@throw ReplyError If precondition is violated
		 *	@throw std::out_of_range If index is invalid
		 */
		Reply const & operator[](std::size_t index) const {
			if (type != ReplyType::array) {
				throw ReplyError({ReplyType::Array}, type);
			}
			
			return array.at(index);
		}
		
}

} // ::redis

