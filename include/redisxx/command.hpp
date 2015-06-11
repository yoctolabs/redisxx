/** @file command.hpp
 *
 * RedisXX Command implementation
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this project source code.
 *
 */
#pragma once
#include <string>
#include <vector>
#include <cstddef>

#include <redisxx/type_traits.hpp>

namespace redis {
namespace priv {

// ----------------------------------------------------------------------------
// API to enable stringification of common non-nested types

// replace bad characters
std::string stringify(std::string value) {
	// tba: escape '\r' and '\n'
	return value;
}

// convert arithmetic type
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
stringify(T value) {
	return std::to_string(value);
}


// ----------------------------------------------------------------------------
// API to apply protocol specifications to various types

// map string to bulk string
std::string protocolify(std::size_t& num, std::string const & value) {
	++num;
	return '$' + std::to_string(value.size()) + "\r\n" + value + "\r\n";
}

// map nullptr_t to null
std::string protocolify(std::size_t& num, std::nullptr_t ptr) {
	++num;
	return "$-1\r\n";
}

// map numbers to bulk string
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
protocolify(std::size_t& num, T value) {
	auto tmp = std::to_string(value);
	++num;
	return '$' + std::to_string(tmp.size()) + "\r\n" + tmp + "\r\n";
}

// map each set to bulk strings
template <typename T>
typename std::enable_if<is_set<T>::value, std::string>::type
protocolify(std::size_t& num, T const & value) {
	// <value0> <value1> ...
	std::string buffer;
	for (auto const & elem: value) {
		// handle each item as simple string
		auto const & tmp = stringify(elem);
		buffer += '$' + std::to_string(tmp.size()) + "\r\n" + tmp + "\r\n";
	}
	num += value.size();
	return buffer;
}

// map each map to bulk strings
template <typename T>
typename std::enable_if<is_map<T>::value, std::string>::type
protocolify(std::size_t& num, T const & value) {
	// <key0> <value0> <key1> <value1> ...
	std::string buffer;
	for (auto const & pair: value) {
		// handle each key and value as simple strings
		auto const & tmp_1 = stringify(pair.first);
		auto const & tmp_2 = stringify(pair.second);
		buffer += '$' + std::to_string(tmp_1.size()) + "\r\n" + tmp_1 + "\r\n"
			+ '$' + std::to_string(tmp_2.size()) + "\r\n" + tmp_2 + "\r\n";
	}
	num += value.size() * 2u;
	return buffer;
}

// map each sequence to bulk strings
template <typename T>
typename std::enable_if<is_sequence<T>::value, std::string>::type
protocolify(std::size_t& num, T const & value) {
	// <value0> <value1> ...
	std::string buffer;
	for (auto const & elem: value) {
		// handle each item as simple string
		auto const & tmp = stringify(elem);
		buffer += '$' + std::to_string(tmp.size()) + "\r\n" + tmp + "\r\n";
	}
	num += value.size();
	return buffer;
}


// ----------------------------------------------------------------------------
// API to dump various types according to the RESP

// base definition of each command
template <typename... Tail>
struct Dump;

// recursive definition of each command
template <typename Head, typename... Tail>
struct Dump<Head, Tail...> {
	
	static inline void process(std::string& out, std::size_t& num, Head&& head, Tail&& ...tail) {
		// push head to array
		out += protocolify(num, std::forward<Head>(head));
		// process tail
		Dump<Tail...>::process(out, num, std::forward<Tail>(tail)...);
	}
	
};

// recursion base case of each command
template <>
struct Dump<> {
	
	static inline void process(std::string& out, std::size_t& num) {
		// nothing left to append
		return;
	}
	
};

} // ::priv


// ----------------------------------------------------------------------------

/// CommandList
class CommandList;

/// Command
/**
 *	This class is used to create RESP-compliant request strings. Each command
 *	instance can hold multiple arguments that belong to one redis command.
 *	Note that each argument that is passed to the Command needs to be atomic.
 *	So "GET foo" as single argument will NOT work to GET the key "foo". Instead
 *	use two arguments "GET" and "foo.
 *	Different argument types are supported: primitive types (such as int, float
 *	and similar), std::string, std::vector<>, std::list<>, std::map<>,
 *	std::unordered_map<>, std::set<>, std::unordered_set<>. Note that those
 *	containers can only contain primitive types or string. Nested types (e.g.
 *	a set of sequences) are NOT supported.
 *
 *	Example usage:
 *	@code
 *		// using primitive types / strings
 *		redis::Command cmd{"SET"};
 *		cmd << "my_key" << 5;
 *		// means: "SET my_key 5"
 *
 *		// using a vector
 *		redis::Command cmd2;
 *		std::vector<int> numbers = {1, 3, 17, 12, 5};
 *		cmd2 << "SADD" << "ids" << numbers;
 *		// means: "SADD ids 1 3 17 12 5"
 *
 *		// using a map
 *		redis::Command cmd3{"HMSET"};
 *		std::map<std::string, std::string> data;
 *		data["name"]	= "max"
 *		data["passwd"]	= "secret"
 *		cmd3 << "user:5" << data;
 *		// means: "HMSET user:5 name max passwd secret"
 *	@endcode
 */
class Command {
	
	friend class CommandList;
	
	template <typename T>
	friend Command& operator<<(Command& cmd, T&& value);
	
	friend bool operator==(Command const & lhs, Command const & rhs);
	friend bool operator!=(Command const & lhs, Command const & rhs);
	
	private:
		std::string buffer;		// preformatted bulk strings
		std::size_t num_bulks;	// number of bulk strings
		
	public:
		/// Construct a new command
		/**
		 *	The ctor can handle a variadic list of arguments and dumps them
		 *	according to RESP. Passing arguments of multiple types is supported
		 *	here.
		 *
		 *	@param ...args variadic list of arguments
		 */
		template <typename ...Args>
		Command(Args&& ...args)
			: buffer{}
			, num_bulks{0} {
			priv::Dump<Args...>::process(buffer, num_bulks, std::forward<Args>(args)...);
		}
		
		/// Return RESP-compliant request string
		/**
		 *	This method can be used to create a RESP-compliant request.
		 *
		 *	@return A ready-to-send request string
		 */
		inline std::string operator*() const {
			return "*" + std::to_string(num_bulks) + "\r\n" + buffer;
		}
		
		/// Clear internal buffer
		/**
		 *	This method can be used to clear the internal command buffer. Note
		 *	that all added values will be lost.
		 */
		inline void clear() {
			buffer.clear();
			num_bulks = 0u;
		}
};

/// Append another value to the command
/**
 *	This method appends a value of a generic type to the given command. The
 *	given argument is dumped according to RESP.
 *
 *	@param cmd Command to append the argument to
 *	@param value Value to append to the command
 *	@param return The given command.
 */
template <typename T>
Command& operator<<(Command& cmd, T&& value) {
	priv::Dump<T>::process(cmd.buffer, cmd.num_bulks, std::forward<T>(value));
	return cmd;
}

/// Check whether to commands equal
/**
 *	Two commands equal if their buffers (and the number of bulk strings inside
 *	the buffers) equal.
 *
 *	@param lhs Left-hand-side command object
 *	@param rhs Right-hand-side command object
 *	@return True if both objects equal
 */
bool operator==(Command const & lhs, Command const & rhs) {
	return (lhs.buffer == rhs.buffer && lhs.num_bulks == rhs.num_bulks);
}

/// Check whether to commands do not equal
/**
 *	Two commands do not equal if their buffers (or the number of bulk strings
 *	inside the buffers) do not equal.
 *
 *	@param lhs Left-hand-side command object
 *	@param rhs Right-hand-side command object
 *	@return True if both objects do not equal
 */
bool operator!=(Command const & lhs, Command const & rhs) {
	return (lhs.num_bulks != rhs.num_bulks || lhs.buffer != rhs.buffer);
}

// ----------------------------------------------------------------------------

/// Type of batch request used for a CommandList
enum class BatchType {
	Pipeline, Transaction
};

/// CommandList
/**
 *	A command list can be used to process multiple commands at once using
 *	pipelining or a transaction. This "batch type" can be modified during
 *	runtime.
 *	Only command objects can be appended to a command list. So the list can be
 *	used to produce a larger RESP-compliant request (either pipelined or using
 *	a transaction).
 *	A limited set of functionality is inherited from std::vector to allow more
 *	detailed access to the underlying container.
 *
 *	Example usage:
 *	@code
 *		redis::CommandList list;
 *		list.reserve(5);
 *		for (int i = 0; i < 5; ++i) {
 *			list << redis::Command{"HGETALL", "user:" + std::to_string(i)};
 *		}
 *
 *		redis::CommandList list2;
 *		list2.setBatchType(redis::BatchType::Transaction);
 *		redis::Command cmd{"PING"};
 *		list2 << cmd << cmd << cmd;
 *		list2.at(1) = redis::Command{"INFO"};
 *	@endcode
 */
class CommandList: private std::vector<Command> {

	friend CommandList& operator<<(CommandList& list, Command const & cmd);

	using Parent = std::vector<Command>;

	private:
		BatchType type;		// determines type of request batch
		
	public:
		/// Construct a new command list
		/**
		 *	This will construct a new command list. Specifying the batch type
		 *	is optional. The default batch type is a transaction.
		 *
		 *	@param type BatchType to use for creating a request string
		 */
		CommandList(BatchType type=BatchType::Transaction)
			: Parent{}
			, type{type} {
		}
		
		/// Returns current batch type
		/**
		 *	Returns whether the current batch type is pipelining or transaction.
		 *
		 *	@return Current BatchType
		 */
		inline BatchType getBatchType() const {
			return type;
		}
		
		/// Set batch type to the given value
		/**
		 *	Sets the batch type to the pipelining or transaction.
		 *
		 *	@param type BatchType to use from now on
		 */
		inline void setBatchType(BatchType type) {
			this->type = type;
		}
		
		/// Return RESP-compliant request string
		/**
		 *	This method can be used to create a RESP-compliant request.
		 *	Depending on the currently set batch type, this string will be a
		 *	pipeline- or transaction-based request.
		 *
		 *	@return A ready-to-send request string
		 */
		inline std::string operator*() const {
			std::string out;
			std::size_t num_bulks = 0u;
			// start command batch
			switch (type) {
				case BatchType::Pipeline:
					out = '*';
					for (auto const & cmd: *this) {
						num_bulks += cmd.num_bulks;
					}
					out += std::to_string(num_bulks) + "\r\n";
					break;
				
				case BatchType::Transaction:
					out = "$5\r\nMULTI\r\n";
					break;
			}
			// concatenate commands
			for (auto const & cmd: *this) {
				out += cmd.buffer;
			}
			// finish command batch
			switch (type) {
				case BatchType::Pipeline:
					// nothing to do
					break;
				
				case BatchType::Transaction:
					out += "$4\r\nEXEC\r\n";
					break;
			}
			return out;
		}
		
		// add some methods of std::vector to the public interface
		using Parent::reserve;
		using Parent::size;
		using Parent::capacity;
		using Parent::clear;
		using Parent::empty;
		using Parent::at;

};

/// Append another command to the list
/**
 *	This method appends a command to the given command list without invalidating
 *	the RESP-compliance of the command list.
 *
 *	@param list CommandList to append the command to
 *	@param cmd Command to append to the list
 *	@param return The given command list.
 */
CommandList& operator<<(CommandList& list, Command const & cmd) {
	list.push_back(cmd);
	return list;
}

} // ::redis

