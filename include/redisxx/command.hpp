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
// API to determine number of strings in a command

inline std::size_t get_size(std::string const & string) {
	// trivial case
	return 1u;
}

// dump any arithmetic type to a string
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, std::size_t>::type
get_size(T value) {
	// also trivial case
	return 1u;
}

template <typename T>
typename std::enable_if<is_map<T>::value, std::size_t>::type
get_size(T const & map) {
	// key and value count seperate
	return map.size() * 2u;
}

template <typename T>
typename std::enable_if<is_set<T>::value, std::size_t>::type
get_size(T const & set) {
	// per element
	return set.size();
}

template <typename T>
typename std::enable_if<is_sequence<T>::value, std::size_t>::type
get_size(T const & sequence) {
	// per element
	return sequence.size();
}

// ----------------------------------------------------------------------------
// API to append data to target vector

// handle a string (trivial case)
inline void append(std::vector<std::string>& out, std::string const & string) {
	out.push_back(string);
}

// handle any arithmetic type
template <typename T>
typename std::enable_if<std::is_arithmetic<T>::value, void>::type
append(std::vector<std::string>& out, T value) {
	append(out, std::to_string(value));
}

// handle any map type
template <typename T>
typename std::enable_if<is_map<T>::value, void>::type
append(std::vector<std::string>& out, T const & map) {
	// append each pair)
	for (auto const & pair: map) {
		append(out, pair.first);
		append(out, pair.second);
	}
}

// handle any set type
template <typename T>
typename std::enable_if<is_set<T>::value, void>::type
append(std::vector<std::string>& out, T const & set) {
	// append each element
	for (auto const & value: set) {
		append(out, value);
	}
}

// handle any sequence type
template <typename T>
typename std::enable_if<is_sequence<T>::value, void>::type
append(std::vector<std::string>& out, T const & sequence) {
	// append each element
	for (auto const & value: sequence) {
		append(out, value);
	}
}

// ---------------------------------------------------------------------------
// Helper API to parse variadic arguments

// base definition of each command
template <typename... Tail>
struct CmdArgs;

// recursive definition of each command
template <typename Head, typename... Tail>
struct CmdArgs<Head, Tail...> {
	
	static inline std::size_t count(Head&& head, Tail&& ...tail) {
		// count actual number of elements in argument list
		return get_size(std::forward<Head>(head))
			+ CmdArgs<Tail...>::count(std::forward<Tail>(tail)...);
	}
	
	static inline void process(std::vector<std::string>& out, Head&& head, Tail&& ...tail) {
		// append head to vector
		append(out, std::forward<Head>(head));
		// process tail
		CmdArgs<Tail...>::process(out, std::forward<Tail>(tail)...);
	}
	
};

// recursion base case of each command
template <>
struct CmdArgs<> {
	
	static inline std::size_t count() {
		// nothing left to count
		return 0u;
	}
	
	static inline void process(std::vector<std::string>& out) {
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
		using buffer = std::vector<std::string>;
		
		buffer elems;
		
	public:
		/// Create command with given arguments
		/**
		 *	@param args variadic list of arguments used to construct the command
		 */
		template <typename ...Args>
		Command(Args&& ...args) {
			// count actual arguments to reserve enough memory at once
			std::size_t size = priv::CmdArgs<Args...>::count(std::forward<Args>(args)...);
			elems.reserve(size);
			// process entire argument list
			priv::CmdArgs<Args...>::process(elems, std::forward<Args>(args)...);
		}
		
		buffer const & operator*() const {
			return elems;
		}
};

/// Add another argument to a command
template <typename T>
Command& operator<<(Command& cmd, T&& arg) {
	// append single argument
	priv::append(cmd.elems, std::forward<T>(arg));
	return cmd;
}

} // ::redis

