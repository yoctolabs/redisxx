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

#include <redisxx/type_traits.hpp>

namespace redis {
namespace priv {

// ----------------------------------------------------------------------------
// API to dump various types to std::string

// dump a string to a string (aka most simple case for dumping values)
inline std::string dump(std::string const & string) {
	return string;
}

// dump any arithmetic type to a string
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
dump(T value) {
	return std::to_string(value);
}

// dump any map type to a string: "<key> <value> <key> <value> [...]"
template <typename T>
typename std::enable_if<is_map<T>::value, std::string>::type
dump(T const & map) {
	std::string s;
	for (auto const & pair: map) {
		if (!s.empty()) {
			s += " ";
		}
		s += dump(pair.first) + " " + dump(pair.second);
	}
	return s;
}

// dump any set type to a string "<value> <value> [...]"
template <typename T>
typename std::enable_if<is_set<T>::value, std::string>::type
dump(T const & set) {
	std::string s;
	for (auto const & value: set) {
		if (!s.empty()) {
			s += " ";
		}
		s += dump(value);
	}
	return s;
}

// dump any sequence type to a string "<value> <value> [...]"
template <typename T>
typename std::enable_if<is_sequence<T>::value, std::string>::type
dump(T const & sequence) {
	std::string s;
	for (auto const & value: sequence) {
		if (!s.empty()) {
			s += " ";
		}
		s += dump(value);
	}
	return s;
}

// ---------------------------------------------------------------------------

// base definition of each command
template <typename... Tail>
struct CommandParser;

// recursive definition of each command
template <typename Head, typename... Tail>
struct CommandParser<Head, Tail...> {
	
	static inline void process(std::string& out, Head&& head, Tail&& ...tail) {
		// push head to array
		if (!out.empty()) {
			out += " ";
		}
		out += dump(std::forward<Head>(head));
		// process tail
		CommandParser<Tail...>::process(out, std::forward<Tail>(tail)...);
	}
	
};

// recursion base case of each command
template <>
struct CommandParser<> {
	
	static inline void process(std::string& out) {
		// nothing left to append
		return;
	}
	
};

} // ::priv

// ---------------------------------------------------------------------------

/// Public command class
class Command {
	
	template <typename T>
	friend Command& operator<<(Command& cmd, T&& value);
	
	private:
		// internal buffer
		std::string buffer;
		
	public:
		/// Construct a new command
		/**
		 *	The ctor can handle a variadic list of arguments, dumps them to
		 *	string and concatenates those strings. Passing arguments of multiple
		 *	types is supported here.
		 *	Supported types are: primitive types (int, float etc.), std::string,
		 *	std::vector<>, std::list<>, std::(unordered_)map<> and
		 *	std::(unordered_)set<> of those supported types.
		 *
		 *	@param ...args variadic list of arguments
		 */
		template <typename ...Args>
		Command(Args&& ...args) {
			priv::CommandParser<Args...>::process(buffer, std::forward<Args>(args)...);
		}
		
		/// Return internal buffer
		/**
		 *	This method can be used to const-access the command buffer. All
		 *	passed arguments were appended to it, each argument is seperated by
		 *	whitespaces.
		 *
		 *	@return const-reference to the internal buffer.
		 */
		std::string const & operator*() const {
			return buffer;
		}
};

/// Append another value to the command
/**
 *	This method appends a value of a generic type to the given command. The
 *	supported types are the same as used for the ctor of `Command`:
 *	primitive types (int, float etc.), std::string, std::vector<>, std::list<>,
 *	std::(unordered_)map<> and std::(unordered_)set<> of those supported types.
 *
 *	@param cmd Command to append the argument to
 *	@param value Value to append to the command
 *	@param return The given command.
 */
template <typename T>
Command& operator<<(Command& cmd, T&& value) {
	if (!cmd.buffer.empty()) {
		cmd.buffer += " ";
	}
	cmd.buffer += priv::dump(std::forward<T>(value));
	return cmd;
}

} // ::redis

