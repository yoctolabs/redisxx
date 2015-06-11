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

/// Public pipeline class
class CommandList;

/// Public command class
class Command {
	
	friend class CommandList;
	
	template <typename T>
	friend Command& operator<<(Command& cmd, T&& value);
	
	private:
		std::string buffer;		// holds preformatted bulk strings
		std::size_t num_bulks;	// number of bulk strings
		
	public:
		/// Construct a new command
		/**
		 *	The ctor can handle a variadic list of arguments and dumps them
		 *	according to RESP. Passing arguments of multiple types is supported
		 *	here. But keep in mind that each argument is handled as an atomic.
		 *	So "GET foo" as one argument will not cause getting they key 'foo'.
		 *	Instead pass two arguments "GET" and "foo".
		 *	Supported types are: primitive types (int, float etc.), std::string,
		 *	std::vector<>, std::list<>, std::(unordered_)map<> and
		 *	std::(unordered_)set<> of those supported types.
		 *	Remember that no nested types (such as a set of sequences) are
		 *	allowed here.
		 *
		 *	@param ...args variadic list of arguments
		 */
		template <typename ...Args>
		Command(Args&& ...args)
			: buffer{}
			, num_bulks{0} {
			priv::Dump<Args...>::process(buffer, num_bulks, std::forward<Args>(args)...);
		}
		
		/// Return internal buffer
		/**
		 *	This method can be used to const-access the command buffer. The
		 *	returned string already contains the leading '*' etc., so it is
		 *	completly RESP-compliant, so it's ready to be sent.
		 *
		 *	@return non-reference to the internal buffer.
		 */
		inline std::string operator*() const {
			return "*" + std::to_string(num_bulks) + "\r\n" + buffer;
		}
};

/// Append another value to the command
/**
 *	This method appends a value of a generic type to the given command. The
 *	given argument is dumped according to RESP. But keep in mind that each
 *	argument is handled as an atomic. So "GET foo" as one argument will not
 *	cause getting they key 'foo'. Instead call this method twice: first with
 *	"GET" and the second time with "foo".
 *	supported types are the same as used for the ctor of `Command`:
 *	primitive types (int, float etc.), std::string, std::vector<>, std::list<>,
 *	std::(unordered_)map<> and std::(unordered_)set<> of those supported types.
 *	Remember that no nested types (such as a set of sequences) are allowed here.
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

// ----------------------------------------------------------------------------

enum class BatchType {
	Pipeline, Transaction
};

class CommandList {

	friend CommandList& operator<<(CommandList& list, Command const & cmd);

	private:
		BatchType type;
		std::vector<Command> buffer;
		
	public:
		CommandList(BatchType type=BatchType::Transaction)
			: type{type}
			, buffer{} {
		}
		
		inline BatchType getBatchType() const {
			return type;
		}
		
		inline void setBatchType(BatchType type) {
			this->type = type;
		}
		
		inline std::string operator*() const {
			std::string out;
			std::size_t num_bulks = 0u;
			// start command batch
			switch (type) {
				case BatchType::Pipeline:
					out = '*';
					for (auto const & cmd: buffer) {
						num_bulks += cmd.num_bulks;
					}
					out += std::to_string(num_bulks);
					break;
				
				case BatchType::Transaction:
					out = "$5\r\nMULTI\r\n";
					break;
			}
			// concatenate commands
			for (auto const & cmd: buffer) {
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

};

CommandList& operator<<(CommandList& list, Command const & cmd) {
	list.buffer.push_back(cmd);
	return list;
}

} // ::redis

