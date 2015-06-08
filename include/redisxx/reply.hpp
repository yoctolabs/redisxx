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

	friend bool operator==(Reply const & lhs, Reply const & rhs);

	private:
		ReplyType type;
		
		bool status;
		std::string string; // for error and string reply
		std::vector<Reply> array; // for array reply
		
		Reply(ReplyType type, bool status, std::string const & string, std::vector<Reply> const & array)
			: type{type}
			, status{status}
			, string{string}
			, array{array} {
		}
		
		Reply(ReplyType type, bool status, std::string const & string, std::vector<Reply>&& array)
			: type{type}
			, status{status}
			, string{string}
			, array{std::move(array)} {
		}
	
	public:
		/// Factory method for null-reply
		/**
		 *	This calls the private constructor.
		 *	@return Reply instance
		 */
		static Reply createNull() {
			return {ReplyType::Null, false, "", {}};
		}
		
		/// Factory method for status-reply
		/**
		 *	This calls the private constructor.
		 *	@param status Status of the reply
		 *	@return Reply instance
		 */
		static Reply createStatus(bool status) {
			return {ReplyType::Status, status, "", {}};
		}
		
		/// Factory method for string-reply
		/**
		 *	This calls the private constructor.
		 *	@param string Message of the reply
		 *	@return Reply instance
		 */
		static Reply createString(std::string const & string) {
			return {ReplyType::String, false, string, {}};
		}
		
		/// Factory method for error-reply
		/**
		 *	This calls the private constructor.
		 *	@param string Error message of the reply
		 *	@return Reply instance
		 */
		static Reply createError(std::string const & string) {
			return {ReplyType::Error, false, string, {}};
		}
		
		/// Factory method for array-reply
		/**
		 *	This calls the private constructor.
		 *	@param array Array of sub-replies
		 *	@return Reply instance
		 */
		static Reply createArray(std::vector<Reply> const & array) {
			return {ReplyType::Array, false, "", array};
		}
		
		/// Factory method overload for array-reply
		/**
		 *	This calls the private constructor. This overload provides moving
		 *	an array "in".
		 *	@param array Array of sub-replies
		 *	@return Reply instance
		 */
		static Reply createArray(std::vector<Reply>&& array) {
			return {ReplyType::Array, false, "", std::move(array)};
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
				case ReplyType::String:
				case ReplyType::Error:
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
			if (type != ReplyType::Array) {
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
			if (type != ReplyType::Array) {
				throw ReplyError({ReplyType::Array}, type);
			}
			
			return array.at(index);
		}
		
};

/// Comparison operator
/**
 *	Two replies equal if their types and the corresponding properties equal.
 *	@param lhs Left-hand-side instance
 *	@param rhs Right-hand-side instance
 *	@return true if both replies are equal
 */
bool operator==(Reply const & lhs, Reply const & rhs) {
	return (lhs.type == rhs.type
		&& lhs.status == rhs.status
		&& lhs.string == rhs.string
		&& lhs.array == rhs.array);
}

} // ::redis

